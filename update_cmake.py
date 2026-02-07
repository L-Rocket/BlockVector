import os

# --- 项目配置 ---
PROJECT_NAME = "main"
CXX_STANDARD = 17
SRC_DIR = "src"
TEST_DIR = "tests"

# --- 扫描 src/tests 目录 ---
def list_cpp_files(dir_path):
    if not os.path.isdir(dir_path):
        return []
    files = [f for f in os.listdir(dir_path) if f.endswith(".cpp")]
    files.sort()
    return files

src_files = list_cpp_files(SRC_DIR)
tests = list_cpp_files(TEST_DIR)

# --- 识别主程序和公共源文件 ---
main_candidates = [f for f in src_files if f == "main.cpp"]
MAIN_TARGET = os.path.join(SRC_DIR, main_candidates[0]) if main_candidates else ""
common_srcs = [os.path.join(SRC_DIR, f) for f in src_files if f != "main.cpp"]

# --- 拼接 CMakeLists 内容 ---
lines = [
    "cmake_minimum_required(VERSION 3.10)",
    f"project({PROJECT_NAME})",
    f"set(CMAKE_CXX_STANDARD {CXX_STANDARD})",
    "",
    "include_directories(include)",
    "",
    "enable_testing()"
]

# --- 添加测试 ---
for test in tests:
    name = os.path.splitext(test)[0]
    test_path = os.path.join(TEST_DIR, test)
    srcs = " ".join([test_path] + common_srcs)
    lines.append(f"add_executable({name} {srcs})")
    lines.append(f"add_test(NAME {name} COMMAND {name})")
    lines.append("")

# --- 添加主程序 ---
if MAIN_TARGET:
    main_srcs = " ".join([MAIN_TARGET] + common_srcs)
    lines.append(f"add_executable({PROJECT_NAME} {main_srcs})")
    lines.append("")

# --- 写入文件 ---
cmake_content = "\n".join(lines)
with open("CMakeLists.txt", "w", encoding="utf-8") as f:
    f.write(cmake_content)

print("✅ CMakeLists.txt 已生成，包含以下测试：")
for t in tests:
    print("  -", t)
