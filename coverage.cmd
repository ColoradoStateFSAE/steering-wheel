@echo off

for /r %%G in (*.gcda) do del "%%G"

pio test -v -e test_windows

IF EXIST "coverage" (
    rmdir /s /q "coverage"
)

mkdir "coverage"

gcovr -r . --html-details -o ./coverage/coverage_report.html ^
    --exclude ".pio" ^
    --exclude "test" ^
