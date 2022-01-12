#include <cpu/difftest.h>
#include <libgen.h>
#include <unistd.h>

char buf_img_file[512];
char buf_filepath[512];
char buf_cmd[1024];
char g_section[64];

/**
 * @brief 从全局变量img_file中拿到当前运行程序的名称
 * @return const char* 名称
 */
const char *name_from_img_file() {
  static char buf[128];
  if (img_file) {
    strcpy(buf, basename(img_file));
    if (strcmp(buf, "Image") == 0) return buf;
    char *p = buf;
    while (*p) {
      if (*p == '.') *p = '\0';
      p++;
    }
    return buf;
  } else {
    return NULL;
  }
}

/**
 * @brief 从当前程序名称找到对应elf文件
 * @param name 名称
 * @return const char* 文件绝对路径
 */
const char *elf_find_path(const char *name) {
  static char buf[256];
  static char buf_filepath[128];
  sprintf(buf, "find $NEMU_HOME/../ -name \"%s.elf\" | head -n 1", name);
  FILE *f = popen(buf, "r");
  int r = fscanf(f, "%s", buf_filepath);
  assert(r);
  pclose(f);
  return buf_filepath;
}

typedef struct {
  uint32_t addr;
  char name[32];
} SectionNode;

// 升序排列的段列表
SectionNode *sections = NULL;
SectionNode *sections_tail = NULL;

/**
 * @brief 读取elf文件信息，储存到有序列表中
 * @param filepath elf文件路径
 */
void elf_init_info(const char *filepath) {
  static char buf[256];
  char line[1024];
  int line_count = 0;
  char buf_line_number[32], buf_addr[12], buf_unknown[12], buf_type[12],
      buf_area[24], buf_type2[32], buf_1[2], buf_name[32];
  // sprintf(buf, "riscv64-linux-gnu-readelf \"%s\" -s | grep FUNC | wc -l",
  //         filepath);
  sprintf(
      buf,
      "riscv64-linux-gnu-readelf \"%s\" -s | grep \"FUNC\\|NOTYPE\" | wc -l",
      filepath);
  FILE *f = popen(buf, "r");
  int r = fscanf(f, "%d", &line_count);
  assert(r);
  Log("ELF Read %d Sections.", (int)line_count);
  sections = malloc(sizeof(SectionNode) * line_count);
  memset(sections, 0, sizeof(SectionNode) * line_count);
  assert(sections);
  sections_tail = sections;
  // sprintf(buf, "riscv64-linux-gnu-readelf \"%s\" -s | grep FUNC | sort -k 2",
  //         filepath);
  sprintf(buf,
          "riscv64-linux-gnu-readelf \"%s\" -s | grep \"FUNC\\|NOTYPE\" | sort "
          "-k 2",
          filepath);
  system(buf);
  f = popen(buf, "r");
  bool read_elf_head = false;
  while (fgets(line, 1024, f)) {
    //  2103: 80038bb4    12 FUNC    GLOBAL DEFAULT    1 __am_gpu_status
    // Log("Read line: %s", line);
    int ret =
        sscanf(line, "%s%s%s%s%s%s%s%s", buf_line_number, buf_addr, buf_unknown,
               buf_type, buf_area, buf_type2, buf_1, buf_name);
    // Assert((ret == 8 && !read_elf_head), "sscanf for elf info should be 8,
    // ret = %d", ret);
    if (!read_elf_head) {
      read_elf_head = true;
      continue;
    }
    // if (ret != 8) {
    //   Log("sscanf for elf info should be 8, ret = %d", ret);
    // }
    if (ret != 8) continue;
    strcpy(sections_tail->name, buf_name);
    sscanf(buf_addr, "%x", &sections_tail->addr);
// #define CONFIG_READELF_OFFSET 0xc0000040
//     if (sections_tail->addr >= CONFIG_READELF_OFFSET)
//       sections_tail->addr -= CONFIG_READELF_OFFSET;
//     else
//       continue;
//     sections_tail->addr += CONFIG_MBASE;
    // Log("ELF Read: " FMT_WORD " : %s", sections_tail->addr, sections_tail->name);
    sections_tail++;
  }
  sections_tail--;
  SectionNode *p = sections;
  // size_t p_len = 10;
  size_t p_len = CONFIG_EXT_SHOW_ELF_SYMBOL_NUMBER;
  while (p != sections_tail && (p_len--)) {
    if (!p->name || (p->name && !*p->name)) continue;
    // if (!p->addr) continue;
    Log(FMT_WORD " %s", p->addr, p->name);
    p++;
  }
  pclose(f);
}

/**
 * @brief 二分查找找到对应的段名称
 * @param pc 目标地址
 * @param left 左端点
 * @param right 右端点
 * @return size_t 段index
 */
size_t find_section_inner(uint32_t pc, size_t left, size_t right) {
  if (left == right || left + 1 == right) return left;
  size_t mid = (left + right) / 2;
  if (sections[mid].addr <= pc && sections[mid + 1].addr > pc) {
    return mid;
  }
  if (sections[mid].addr < pc) {
    return find_section_inner(pc, mid, right);
  } else {
    return find_section_inner(pc, left, mid);
  }
}

/**
 * @brief 查找段名称
 * @param pc 目标地址
 * @return const char* 段名称
 */
const char *find_section(uint32_t pc) {
  static char buf[64];
  if (!sections) return "--";
  if (pc < sections[0].addr) return "HEAD";
  if (pc >= sections_tail->addr) return "TAIL";
  SectionNode *t =
      &sections[find_section_inner(pc, 0, (sections_tail - sections - 1))];
  if (pc != t->addr)
    sprintf(buf, "%s+0x%x", t->name, (pc - t->addr));
  else
    sprintf(buf, "<%s>", t->name);
  // return sections[find_section_inner(pc, 0, (sections_tail - sections - 1))]
  //     .name;
  return buf;
}

/**
 * @brief 初始化
 */
void elf_init() {
  const char *name = name_from_img_file();
  if (!name) {
    Log("No elf file to read!");
    return;
  }
  // assert(name);
  const char *path = NULL;
  if (strcmp(name, "Image") == 0) {
    path = "/home/chiro/Downloads/linux-5.4.168/vmlinux";
  } else {
    path = elf_find_path(name);
  }
  assert(path);
  Log("ELF path: %s", path);
  elf_init_info(path);
}