# stack_constructor

> Динамический стек целых чисел с канарейками, хешами и дампами. ВИииииииииа


## Аннотация
`stack_constructor` — это не просто стек. Это стек, созданный по стандартам, одобренным 3\2шкой.

Особенности стека:
- хранит указатели на собственные методы (`push`, `pop`, `extend`, `merge`, `fill`, `destroy`) — тот самый ооп-стайл по которому соскучились все сишники;
- окружен канарейками по всем стандартам безопасного кода(`KANARY_MODE`);
- умеет находить неоиданные сюрпризы в данных через хеш `djb2` (`HASH_MODE`);
- умеет сообщать, что с ним случилось, через дампы и логи (`DEBUG_MODE`);
- динамически расширяется до ближайшей степени двойки и сжимается, когда проходит 2 степени (чтобы загружение любого колличества ваших фоточек с котиками).

## Подключение библиотеки

Подключи `stack_constructor.h` и `stack_constructor.cpp` к своему проекту.

**Минимальный пример:**

```c
#include "stack_constructor.h"

int main(void) {
    StackConstructor st;
    _init_(4, &st, 10, 20, 30);   // стек: [10, 20, 30]

    st.push(&st, 42);             // [10, 20, 30, 42]
    st.extend(&st, 2, 7, 8);      // [10, 20, 30, 42, 7, 8]
    st.pop(&st, 0);               // [10, 20, 30, 42, 7]

    _stack_display(&st);
    st.destroy(&st);
    return 0;
}
```


## Режимы сборки

Режимы включаются макросами препроцессора. Можно комбинировать.

`DEBUG_MODE` - Дампы в `logs.txt`, логирование вызовов, печать причин ошибок
`KANARY_MODE` - Оборачивает данные двумя «канарейками» (`INFINITY`) и проверяет их на каждой операции
`HASH_MODE` - Считает djb2-хеш содержимого и сверяет его при каждой проверке

**Пример сборки со всеми защитами:**

```bash
g++ main.cpp stack_constructor.cpp -o a.exe -DDEBUG_MODE -DKANARY_MODE -DHASH_MODE
```

## API

### Структура `StackConstructor`

`lli` - long long int

### Структура `StackConstructor`

| Поле | Тип | Описание |
|---|---|---|
| `stack` | `StackValue*` | Указатель на массив элементов |
| `hardware_stack` | `void*` | Указатель на массив с канарейками (`KANARY_MODE`) |
| `size` | `lli` | Текущее количество элементов |
| `capacity` | `lli` | Вместимость |
| `alive` | `bool` | Можно ли обращаться к стеку (НЕ УБИВАЕТ СТЕК, отладочная информация) |
| `hash` | `size_t` | Хеш содержимого (`HASH_MODE`) |
| `name`, `created_by` | `char*` | Отладочная информация (`DEBUG_MODE`) |

### Методы

| Метод | Сигнатура | Описание |
|---|---|---|
| `push` | `errno_t push(StackConstructor*, StackValue)` | Добавить элемент |
| `pop` | `errno_t pop(StackConstructor*, lli)` | Получить последний элемент |
| `extend` | `errno_t extend(StackConstructor*, size_t argc, ...)` | Добавить сразу несколько элементов |
| `fill` | `errno_t fill(StackConstructor*, lli leng, StackValue val)` | Заполнить стек `leng` значениями `val` |
| `merge` | `errno_t merge(StackConstructor*, StackConstructor*, StackConstructor*)` | Склеить два стека в третий |
| `destroy` | `errno_t destroy(StackConstructor*)` | Уничтожает стек |

### Функции

| Функция | Описание |
|---|---|
| `#define _init_stack(StackConstructor*, ...)` | Надо передать какой стек инитить, а потом его первоначальные элементы (не больше 9)|
| `_stack_display(StackConstructor*)` | Напечатать все элементы в `stdout` |
| `_el_in_stack(StackConstructor*, StackValue)` | Проверяем, в стеке ли элемент |
| `_stacks_equal(StackConstructor*, StackConstructor*)` | возвращает `true` если стеки полностью равны |

### Дампы (`DEBUG_MODE`)

При любом подозрительном событии в `logs.txt` пишется подробный снимок:

```
STACK_DUMP	[22:46:52]
 called in __check_if_OK__ [stack_constructor.cpp:90] {
	[adress = 000000dba39ffa60]
	stk -> capacity = 16
	stk -> size = 10
	stk -> hash = 8241475877233892380
	stk: left_canary -> inf (etalon = inf)
	stk -> stack [adr: 0000020b5a551578] {
		[0] : < 1 >
		[1] : < 4 >
		[2] : < 2 >
		[3] : < -3 >
		[4] : < 3 >
		[5] : < 3 >
		[6] : < -2 >
		[7] : < 3 >
		[8] : < 4 >
		[9] : < 7 >
	}
	stk: righ_canary -> inf (etalon = inf)
}
```

## Oсобенности

- **Windows-only.** Функция `is_heap_pointer` исполььзует `<windows.h>`.
- Проверки активны только в `DEBUG_MODE`. Без него `_check_if_OK_` почти ничего не делает.
