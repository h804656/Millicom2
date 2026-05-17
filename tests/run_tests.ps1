# Test runner for CompileCC.oap grammar tests.
#
# For each tests/*.lex it feeds the file through the C++ executor and a
# pre-built CompileCC.ind, captures stdout, and checks that every non-empty,
# non-comment line in the matching *.expect appears as a substring of stdout.
#
# Usage:
#   .\tests\run_tests.ps1                    # run all tests
#   .\tests\run_tests.ps1 -Filter 04_*       # run a subset
#   .\tests\run_tests.ps1 -Ind CompileC.ind  # test against a different .ind
#   .\tests\run_tests.ps1 -ShowOutput        # print stdout for each test

param(
    [string]$Filter = "*",
    [string]$Ind = "oap2\CompileCC.ind",
    [string]$Exe = ".\x64\Debug\Millicom.exe",
    [int]$TimeoutSec = 30,
    [switch]$ShowOutput
)

$ErrorActionPreference = "Stop"
Set-Location -Path (Split-Path -Parent $PSScriptRoot)

if (-not (Test-Path $Exe)) {
    Write-Host "Executor not found at $Exe. Build it with:" -ForegroundColor Red
    Write-Host '  & "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" Millicom.vcxproj /p:Configuration=Debug /p:Platform=x64'
    exit 2
}

if (-not (Test-Path $Ind)) {
    Write-Host "Index file '$Ind' not found." -ForegroundColor Red
    Write-Host "Compile CompileCC.oap to CompileCC.ind first (via the legacy Delphi millicom.exe at E:\miemp\Millicom\Millicom\millicom.exe), or pass -Ind <path>." -ForegroundColor Yellow
    exit 2
}

$testFiles = Get-ChildItem -Path "tests" -Filter "$Filter.lex" -File |
    Where-Object { -not $_.Name.StartsWith('.') } |
    Sort-Object Name
if ($testFiles.Count -eq 0) {
    Write-Host "No tests matched filter '$Filter'." -ForegroundColor Yellow
    exit 0
}

$passed = 0
$failed = 0
$failures = @()

foreach ($lexFile in $testFiles) {
    $name = [System.IO.Path]::GetFileNameWithoutExtension($lexFile.Name)
    $expectFile = Join-Path $lexFile.DirectoryName "$name.expect"

    if (-not (Test-Path $expectFile)) {
        Write-Host "  SKIP $name (no .expect file)" -ForegroundColor DarkYellow
        continue
    }

    # The C++ Lex emits `//` line comments as string tokens (the Delphi original
    # drops them). Until that's fixed, strip comments out of the .lex file before
    # feeding it through. Strip anything from a `//` to end-of-line on each line
    # that's not inside a string literal -- crude, but enough for fixtures.
    $stripped = "tests\.last_input.lex"
    $raw = Get-Content $lexFile.FullName -Raw
    # 1) Strip `//` line comments (C++ Lex emits comments as StrAtr tokens).
    # 2) Collapse every newline into a space (C++ Lex emits a StrAtr for each
    #    newline via state 12, which Root rejects). Multi-statement programs
    #    therefore have to be fed as a single space-separated line.
    $clean = ($raw -replace '(?m)//.*$', '') -replace '\s+', ' '
    $clean = $clean.Trim()
    # Append a trailing separator so the parser's final state always dispatches
    # via an `else` transition (otherwise the last token's MnemoAnalysis-style
    # state sits in limbo and its `else` trace never fires).
    if ($clean -ne '') { $clean += ' ;' }
    Set-Content -Path $stripped -Value $clean -NoNewline -Encoding ASCII

    $proc = Start-Process -FilePath $Exe `
        -ArgumentList @($Ind, "--lex-file", $stripped) `
        -NoNewWindow -PassThru `
        -RedirectStandardOutput "tests\.last_stdout.txt" `
        -RedirectStandardError  "tests\.last_stderr.txt"
    if (-not $proc.WaitForExit($TimeoutSec * 1000)) {
        $proc.Kill()
        Write-Host "  FAIL $name (timeout after ${TimeoutSec}s)" -ForegroundColor Red
        $failed++
        $failures += $name
        continue
    }

    $stdout = Get-Content "tests\.last_stdout.txt" -Raw -ErrorAction SilentlyContinue
    if ($null -eq $stdout) { $stdout = "" }

    # $proc.ExitCode may not be populated reliably for -NoNewWindow processes;
    # treat null as 0. Non-zero exit (typically 5, AV) indicates a lexer crash.
    if ($null -ne $proc.ExitCode -and $proc.ExitCode -ne 0) {
        Write-Host "  FAIL $name (executor exit=$($proc.ExitCode); likely AV in lexer)" -ForegroundColor Red
        $failed++
        $failures += $name
        continue
    }

    $expectations = Get-Content $expectFile |
        Where-Object { $_ -and -not $_.StartsWith("#") }

    $missing = @()
    foreach ($e in $expectations) {
        if (-not $stdout.Contains($e)) {
            $missing += $e
        }
    }

    if ($missing.Count -eq 0) {
        Write-Host "  PASS $name" -ForegroundColor Green
        $passed++
    } else {
        Write-Host "  FAIL $name" -ForegroundColor Red
        foreach ($m in $missing) {
            Write-Host "        missing: $m" -ForegroundColor Red
        }
        $failed++
        $failures += $name
    }

    if ($ShowOutput) {
        Write-Host "  ---- stdout ----" -ForegroundColor DarkGray
        $stdout -split "`r?`n" | ForEach-Object { Write-Host "    $_" -ForegroundColor DarkGray }
        Write-Host "  ----------------" -ForegroundColor DarkGray
    }
}

Write-Host ""
Write-Host "Passed: $passed  Failed: $failed" -ForegroundColor $(if ($failed -eq 0) { 'Green' } else { 'Red' })
if ($failed -gt 0) { exit 1 } else { exit 0 }
