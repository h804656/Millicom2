# End-to-end ALE test runner.
#
# For each tests/ale_*.oap:
#   1. compile to .ind via CompileCC.ind
#   2. run the .ind and capture stdout
#   3. line-by-line compare stdout to tests/ale_*.expect
#
# Unlike run_tests.ps1 (which substring-matches parser traces), this runs
# the *compiled output* and checks the program's actual runtime printout.
#
# Usage:
#   .\tests\run_ale.ps1                       # all ale tests
#   .\tests\run_ale.ps1 -Filter 03_*          # subset
#   .\tests\run_ale.ps1 -ShowOutput           # dump compile log + run output

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
    Write-Host "Executor not found at $Exe" -ForegroundColor Red
    exit 2
}
if (-not (Test-Path $Ind)) {
    Write-Host "CompileCC.ind not found at $Ind" -ForegroundColor Red
    exit 2
}

$testFiles = Get-ChildItem -Path "tests" -Filter "ale_$Filter.oap" -File |
    Sort-Object Name
if ($testFiles.Count -eq 0) {
    Write-Host "No ale tests matched filter 'ale_$Filter'." -ForegroundColor Yellow
    exit 0
}

$passed = 0; $failed = 0; $failures = @()

foreach ($oap in $testFiles) {
    $name = [System.IO.Path]::GetFileNameWithoutExtension($oap.Name)
    $expectFile = Join-Path $oap.DirectoryName "$name.expect"
    if (-not (Test-Path $expectFile)) {
        Write-Host "  SKIP $name (no .expect)" -ForegroundColor DarkYellow
        continue
    }

    $compiledInd = "tests\.last_ale.ind"

    # 1. Compile the .oap to a .ind (the loaded .ind self-contains the compiler).
    $compileStdout = "tests\.last_ale_compile.txt"
    $proc = Start-Process -FilePath $Exe `
        -ArgumentList @($Ind, "--lex-file", $oap.FullName, "--out-file", $compiledInd) `
        -NoNewWindow -PassThru `
        -RedirectStandardOutput $compileStdout `
        -RedirectStandardError "tests\.last_ale_compile_err.txt"
    if (-not $proc.WaitForExit($TimeoutSec * 1000)) {
        $proc.Kill()
        Write-Host "  FAIL $name (compile timeout)" -ForegroundColor Red
        $failed++; $failures += $name
        continue
    }
    if (-not (Test-Path $compiledInd)) {
        Write-Host "  FAIL $name (compile produced no .ind)" -ForegroundColor Red
        $failed++; $failures += $name
        continue
    }

    # 2. Run the compiled .ind
    $runStdout = "tests\.last_ale_run.txt"
    $proc = Start-Process -FilePath $Exe `
        -ArgumentList @($compiledInd) `
        -NoNewWindow -PassThru `
        -RedirectStandardOutput $runStdout `
        -RedirectStandardError "tests\.last_ale_run_err.txt"
    if (-not $proc.WaitForExit($TimeoutSec * 1000)) {
        $proc.Kill()
        Write-Host "  FAIL $name (run timeout)" -ForegroundColor Red
        $failed++; $failures += $name
        continue
    }
    $actual = Get-Content $runStdout -ErrorAction SilentlyContinue
    if ($null -eq $actual) { $actual = @() }
    elseif ($actual -is [string]) { $actual = @($actual) }

    # 3. Compare stdout to .expect, line-by-line, ignoring blank lines at end
    $expected = Get-Content $expectFile -ErrorAction SilentlyContinue
    if ($null -eq $expected) { $expected = @() }
    elseif ($expected -is [string]) { $expected = @($expected) }
    while ($actual.Count -gt 0 -and $actual[-1] -eq '') {
        $actual = $actual[0..($actual.Count - 2)]
    }
    while ($expected.Count -gt 0 -and $expected[-1] -eq '') {
        $expected = $expected[0..($expected.Count - 2)]
    }

    $ok = $true
    if ($actual.Count -ne $expected.Count) { $ok = $false }
    else {
        for ($i = 0; $i -lt $actual.Count; $i++) {
            if ($actual[$i] -ne $expected[$i]) { $ok = $false; break }
        }
    }

    if ($ok) {
        Write-Host "  PASS $name" -ForegroundColor Green
        $passed++
    } else {
        Write-Host "  FAIL $name" -ForegroundColor Red
        Write-Host "    expected ($($expected.Count) lines):" -ForegroundColor DarkGray
        $expected | ForEach-Object { Write-Host "      $_" -ForegroundColor DarkGray }
        Write-Host "    got ($($actual.Count) lines):" -ForegroundColor DarkGray
        $actual | ForEach-Object { Write-Host "      $_" -ForegroundColor DarkGray }
        $failed++; $failures += $name
    }

    if ($ShowOutput) {
        Write-Host "  ---- compile stdout ----" -ForegroundColor DarkGray
        Get-Content $compileStdout | ForEach-Object { Write-Host "    $_" -ForegroundColor DarkGray }
    }
}

Write-Host ""
Write-Host "Passed: $passed  Failed: $failed" -ForegroundColor $(if ($failed -eq 0) { 'Green' } else { 'Red' })
if ($failed -gt 0) { exit 1 } else { exit 0 }
