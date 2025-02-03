# Контрольная работа по дисциплине **"Разработка программных систем"**
**Исполнитель:** Торопов Максим Алексеевич  
**Группа:** 4292

## Зависимости
* CMake версии >= 3.20
* Компилятор

## Сборка
```
cmake -B build [-DBUILD_WITH_TESTS=TRUE]
cmake --build build
```
Флаг BUILD_WITH_TESTS является опциональным, при выставлении которого также будут собраны тесты.

## Запуск
После сборки необходимо запустить файл `build/src/cwork`  
Чтобы запустить тесты (при условии, что они были собраны), необходимо запустить файл `build/test/tests`
