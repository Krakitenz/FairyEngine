[CmdletBinding()]
param(
	[ValidateSet('Debug', 'Release')]
	[string]$Configuration = 'Debug'
)

$ErrorActionPreference = 'Stop'

$root = Split-Path $PSScriptRoot -Parent
$testExecutable = Join-Path $root "tests\bin\x64\$Configuration\FairyEngine.Tests.exe"
if (-not (Test-Path -LiteralPath $testExecutable -PathType Leaf)) {
	throw "Native test executable not found: $testExecutable. Build FairyEngine.Tests.vcxproj for x64 $Configuration first."
}

Push-Location $root
try {
	& $testExecutable
	$exitCode = $LASTEXITCODE
	if ($exitCode -ne 0) {
		throw "Native test suite failed with exit code $exitCode"
	}
}
finally {
	Pop-Location
}
