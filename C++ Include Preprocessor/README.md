# C++ Include Preprocessor

Утилита на C++17, которая рекурсивно подставляет содержимое файлов на место директив `#include` — упрощённый аналог этапа включения файлов в препроцессоре C++.

## Возможности

- Обработка двух видов директив:
  - `#include "file"` — поиск относительно папки текущего файла, затем по include-директориям;
  - `#include <file>` — поиск только по include-директориям.
- Рекурсивная обработка: включённые файлы тоже раскрываются, содержимое подставляется ровно на место директивы.
- Устойчивость к нестандартным пробелам и табуляциям (`#   include<file>`, `\t#\tinclude\t"file"`).
- Сообщение об ошибке с указанием файла и номера строки, если включаемый файл не найден:
  ```
  unknown include file dummy.txt at file sources/a.cpp at line 8
  ```
- Если входной файл не открывается, выходной файл не создаётся.

## Интерфейс

```cpp
bool Preprocess(const path& in_file, const path& out_file,
                const vector<path>& include_directories);
```

| Параметр | Описание |
|---|---|
| `in_file` | исходный файл |
| `out_file` | файл, куда записывается результат |
| `include_directories` | папки для поиска, проверяются по порядку |

Возвращает `true`, если все включаемые файлы найдены, и `false` при ошибке. При ошибке в выходном файле остаются все строки, записанные до неё.

## Пример

Структура файлов:

```
sources/
├── a.cpp
├── dir1/
│   ├── b.h            #include "subdir/c.h"
│   ├── d.h            #include "lib/std2.h"
│   └── subdir/
│       └── c.h        #include <std1.h>
├── include1/
│   └── std1.h
└── include2/
    └── lib/
        └── std2.h
```

`sources/a.cpp`:

```cpp
// this comment before include
#include "dir1/b.h"
// text between b.h and c.h
#include "dir1/d.h"

int SayHello() {
    cout << "hello, world!" << endl;
#   include<dummy.txt>
}
```

Вызов:

```cpp
Preprocess("sources/a.cpp", "sources/a.in",
           {"sources/include1", "sources/include2"});
```

Результат в `sources/a.in`:

```cpp
// this comment before include
// text from b.h before include
// text from c.h before include
// std1
// text from c.h after include
// text from b.h after include
// text between b.h and c.h
// text from d.h before include
// std2
// text from d.h after include

int SayHello() {
    cout << "hello, world!" << endl;
```

Вывод в консоль (файла `dummy.txt` нет, обработка остановлена):

```
unknown include file dummy.txt at file sources/a.cpp at line 8
```

## Сборка и запуск

```bash
clang++ -std=c++17 -Wall -Wextra include_preprocessor.cpp -o preprocessor
./preprocessor
```

Программа запускает встроенный тест `Test()`: создаёт пример из раздела выше в папке `sources/` и проверяет результат через `assert`.

## Технологии

- C++17
- `std::filesystem` — работа с путями и проверка существования файлов
- `std::regex` — разбор директив `#include`
- `std::ifstream` / `std::ofstream` — построчное чтение и запись
- рекурсия для обработки вложенных включений

## Ограничения

- Обрабатываются только `#include`; `#define`, условная компиляция и макросы не поддерживаются.
- Предполагается, что во входных файлах нет циклических включений.
