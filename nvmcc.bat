@echo off
REM compiler-assembler-linker pipeline
tclsh nvmascc.tcl | tclsh nvmasas.tcl | tclsh nvmasln.tcl >a.out

SET /P		"IBOOL=Da li zelite da pokrenete program? (d/n):"

IF "%IBOOL%"=="d" vmas.exe <a.out