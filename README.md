# TuDienGUI — Từ điển Anh - Việt (Dear ImGui)

Ứng dụng từ điển Anh - Việt có giao diện đồ họa, viết bằng C++17 và Dear ImGui.
Gồm 4 tính năng chính: **Tra từ điển**, **Lịch sử tra cứu**, **Trắc nghiệm từ vựng (Quiz)**, và **Từ đồng nghĩa**.

## 1. Tính năng

### 📖 Tab Dictionary (Tra từ điển)
- Gõ từ tiếng Anh vào ô tìm kiếm → hiện nghĩa tiếng Việt, từ loại (nếu có) và câu ví dụ (nếu có).
- **Live Search**: nếu chưa gõ đủ, ứng dụng gợi ý danh sách từ có cùng tiền tố (dựa trên cấu trúc Trie).
- **Did you mean**: nếu gõ sai chính tả và không tìm thấy tiền tố nào khớp, ứng dụng gợi ý các từ gần đúng nhất (Levenshtein Distance).
- **Thêm từ mới**: nhập từ tiếng Anh, từ loại (tùy chọn), nghĩa tiếng Việt, câu ví dụ (tùy chọn) → bấm **Add word**.
- **Xóa từ**: nhập từ cần xóa → bấm **Delete**.

### 🕘 Tab Search History (Lịch sử tra cứu)
- Hiển thị tối đa 10 từ tra cứu gần nhất (mới nhất ở trên cùng), theo kiểu LRU cache.
- Tra lại một từ đã có trong lịch sử sẽ đưa từ đó lên đầu danh sách thay vì tạo bản ghi trùng.

### 📝 Tab Quiz (Trắc nghiệm từ vựng)
- Câu hỏi được nạp tự động từ từ điển, mỗi câu có 4 đáp án (A/B/C/D), trong đó có 1 đáp án đúng.
- **Use Hint**: loại bỏ ngẫu nhiên 1 đáp án sai (giữ lại ít nhất 1 đáp án sai để không lộ đáp án đúng).
- Trả lời đúng → chuyển sang câu mới. Trả lời sai → câu hỏi được đưa lại vào cuối hàng đợi để hỏi lại sau (spaced repetition).
- **Quiz history**: xem lại toàn bộ lịch sử các câu đã trả lời (từ, đáp án đúng, đáp án đã chọn).

### 🔄 Tab Synonyms (Từ đồng nghĩa)
- Gõ một từ tiếng Anh → hiện nghĩa tiếng Việt của từ đó và danh sách các từ đồng nghĩa.

## 2. Cấu trúc thư mục

```
.
├── CMakeLists.txt        # Cấu hình build
├── vcpkg.json             # Khai báo thư viện phụ thuộc (imgui, glfw3, opengl)
├── main.cpp                # Khởi tạo cửa sổ GLFW + Dear ImGui, vòng lặp render chính
├── Dictionary.h             # Khai báo toàn bộ struct/class: Trie, SearchHistory, Vocabulary
├── Trie.cpp                  # Cài đặt Trie: insert/search/xóa/gợi ý tiền tố/gợi ý chính tả
├── History.cpp                # Cài đặt danh sách liên kết đôi cho lịch sử tra cứu (LRU)
├── Quiz.cpp                    # Cài đặt Queue (mảng vòng) + Stack (danh sách liên kết) cho Quiz
├── Synonym.h / Synonym.cpp      # Từ điển đồng nghĩa (unordered_map)
├── Platform.cpp                  # Các hàm phụ thuộc hệ điều hành (đọc phím, xóa màn hình...)
├── Gui.cpp                        # Toàn bộ giao diện Dear ImGui (4 tab)
├── tudien.txt                      # Dữ liệu từ điển chính
├── dongnghia.txt                    # Dữ liệu từ đồng nghĩa
└── imgui.ini                         # File lưu vị trí/kích thước cửa sổ (tự sinh, có thể xóa)
```

## 3. Định dạng file dữ liệu

### `tudien.txt` (từ điển chính)
Mỗi dòng theo định dạng:
```
tu|tu_loai|nghia|vi_du
```
- `tu_loai` và `vi_du` có thể để trống, nhưng vẫn phải giữ đủ dấu `|`.
- Ví dụ:
```
apple|(n)|quả táo|I eat an apple every day.
run||chạy|
```

### `dongnghia.txt` (từ đồng nghĩa)
Mỗi dòng theo định dạng:
```
tu|nghia|tu_dong_nghia_1, tu_dong_nghia_2, tu_dong_nghia_3
```
- Ví dụ:
```
happy|vui vẻ, hạnh phúc|joyful, cheerful, glad
```

> Cả hai file phải đặt cùng thư mục với file `.exe` sau khi build (CMake đã tự động copy 2 file này sau mỗi lần build, xem `CMakeLists.txt`).

## 4. Yêu cầu môi trường

- CMake ≥ 3.15
- Trình biên dịch hỗ trợ C++17 (MSVC / g++ / clang++)
- [vcpkg](https://github.com/microsoft/vcpkg) để cài các thư viện:
  - `imgui` (kèm feature `glfw-binding`, `opengl3-binding`)
  - `glfw3`
  - `opengl`

## 5. Cách build & chạy

### Bước 1 — Cài thư viện qua vcpkg
```bash
vcpkg install imgui[glfw-binding,opengl3-binding] glfw3 opengl
```
Hoặc nếu project có sẵn `vcpkg.json`, chỉ cần bật chế độ manifest (vcpkg sẽ tự đọc file này khi cấu hình CMake).

### Bước 2 — Cấu hình CMake (trỏ tới toolchain của vcpkg)
```bash
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=[đường-dẫn-tới-vcpkg]/scripts/buildsystems/vcpkg.cmake
```

### Bước 3 — Build
```bash
cmake --build build --config Release
```

### Bước 4 — Chạy
File thực thi `TuDienGUI` (hoặc `TuDienGUI.exe` trên Windows) sẽ nằm trong thư mục `build`. Chạy trực tiếp file này; `tudien.txt` và `dongnghia.txt` đã được tự động copy vào cùng thư mục.

> **Lưu ý (Windows):** `main.cpp` đang nạp font từ đường dẫn cứng `C:\Windows\Fonts\arial.ttf` để hiển thị được tiếng Việt có dấu. Nếu build trên hệ điều hành khác Windows, cần đổi đường dẫn này sang một file font `.ttf` có sẵn trên máy (ví dụ font DejaVu Sans trên Linux) để tránh lỗi khi khởi động hoặc mất dấu tiếng Việt.

## 6. Ghi chú kỹ thuật nhanh

- **Trie**: hỗ trợ 28 ký tự (a-z, dấu cách, dấu gạch ngang) để tra được cả cụm từ nhiều tiếng.
- **Lịch sử tra cứu**: danh sách liên kết đôi, giới hạn 10 mục gần nhất, hoạt động theo cơ chế LRU.
- **Quiz**: hàng đợi câu hỏi cài bằng mảng vòng (sức chứa cố định 10 câu), lịch sử trả lời cài bằng stack (danh sách liên kết đơn).
- **Từ đồng nghĩa**: tra cứu bằng `unordered_map`, trung bình O(1).
