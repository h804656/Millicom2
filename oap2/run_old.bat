@echo off
setlocal EnableDelayedExpansion

rem ============================================================================
rem run_old.bat -- compile an .oap source to .ind and then execute the .ind
rem                via the legacy Delphi millicom.exe (E:\miemp\...). Mirrors
rem                the compile+run loop of run.bat, but everything goes through
rem                the Delphi compiler instead of the C++ executor.
rem
rem Note: Delphi's --compile-to flag only produces a meaningful .ind for source
rem       files that set up their own CapsManager (NewFU declarations alone
rem       don't capture). If the .ind comes back empty/missing this script
rem       falls back to a plain --run so the program still executes.
rem
rem Usage:    run_old.bat [--fast] <source.oap>
rem Example:  run_old.bat ..\oap2\CompileCC.oap
rem           run_old.bat --fast CompileCC.oap
rem ============================================================================

set "OAPDIR=%~dp0"
set "ROOT=%OAPDIR%.."
set "DELPHI=E:\miemp\Millicom\Millicom\millicom.exe"
set "DELPHIDIR=E:\miemp\Millicom\Millicom"
set "EMPTY=%ROOT%\tests\empty.oap"

rem ---- Parse args ------------------------------------------------------------
set "FAST="
set "SRC="
:parse_args
if "%~1"=="" goto args_done
if /i "%~1"=="--fast" (
  set "FAST=1"
  shift
  goto parse_args
)
if /i "%~1"=="-f" (
  set "FAST=1"
  shift
  goto parse_args
)
if "%SRC%"=="" (
  set "SRC=%~f1"
  shift
  goto parse_args
)
echo Error: unexpected argument %1
exit /b 1
:args_done

if "%SRC%"=="" (
  echo Usage: %~nx0 [--fast^|-f] ^<source.oap^>
  echo.
  echo   Without --fast:  compile ^<source.oap^> -^> ^<source.ind^> via Delphi,
  echo                    then execute the result via Delphi and print stdout.
  echo   With --fast:     skip the compile and reuse the existing .ind.
  exit /b 1
)
if not exist "%SRC%" (
  echo Error: source file not found: %SRC%
  exit /b 1
)
if not exist "%DELPHI%" (
  echo Error: Delphi millicom.exe not found at %DELPHI%
  exit /b 1
)
if not exist "%EMPTY%" (
  echo Error: %EMPTY% not found ^(needed as --run dummy for --compile-to^)
  exit /b 1
)

rem Output .ind sits next to the source, same basename.
for %%I in ("%SRC%") do set "IND=%%~dpnI.ind"

rem ---- 1. Compile .oap -> .ind (unless --fast) -------------------------------
if defined FAST (
  if not exist "%IND%" (
    echo Error: --fast given but %IND% does not exist. Run without --fast first.
    exit /b 2
  )
  echo [skip] Compile skipped ^(--fast^); reusing existing %IND%
) else (
  echo [1/2] Compiling
  echo        from: %SRC%
  echo        to:   %IND%
  rem Drop any prior .ind so we can detect compile failures by its absence.
  if exist "%IND%" del /q "%IND%"
  set "CLOG=%TEMP%\millicom_compile_%RANDOM%.log"
  if exist "!CLOG!" del /q "!CLOG!"
  rem millicom.exe --exit AV-loops if CWD isn't its install dir (CursorTimer
  rem hits an empty TPageControl). pushd before invoking.
  pushd "%DELPHIDIR%"
  start "millicom-compile" /wait "%DELPHI%" --bootstrap "%SRC%" --run "%EMPTY%" --compile-to "%IND%" --out "!CLOG!" --exit-delay 8000 --exit
  popd
  if exist "!CLOG!" (
    echo ---- compile messages ----
    type "!CLOG!"
    del /q "!CLOG!" 2>NUL
  )
  if not exist "%IND%" (
    echo.
    echo [warn] %IND% was not produced ^(source likely lacks CapsManager prelude^).
    echo        Falling back to inline --run so the program still executes.
    goto fallback_run
  )
)

rem ---- 2. Execute the .ind via Delphi --exec-ind -----------------------------
echo [2/2] Executing %IND%
echo ---- stdout ----
set "RLOG=%TEMP%\millicom_run_%RANDOM%.log"
if exist "%RLOG%" del /q "%RLOG%"
pushd "%DELPHIDIR%"
start "millicom-run" /wait "%DELPHI%" --exec-ind "%IND%" --out "%RLOG%" --exit-delay 8000 --exit
set "RC=!ERRORLEVEL!"
popd
if exist "%RLOG%" (
  type "%RLOG%"
  del /q "%RLOG%" 2>NUL
)
echo ---- end (exit=!RC!) ----
exit /b !RC!

:fallback_run
echo [2/2] Running %SRC% inline ^(no .ind^)
echo ---- stdout ----
set "RLOG=%TEMP%\millicom_run_%RANDOM%.log"
if exist "%RLOG%" del /q "%RLOG%"
pushd "%DELPHIDIR%"
start "millicom-run" /wait "%DELPHI%" --run "%SRC%" --out "%RLOG%" --exit-delay 8000 --exit
set "RC=!ERRORLEVEL!"
popd
if exist "%RLOG%" (
  type "%RLOG%"
  del /q "%RLOG%" 2>NUL
)
echo ---- end (exit=!RC!) ----
exit /b !RC!
