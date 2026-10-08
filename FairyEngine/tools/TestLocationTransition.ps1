$ErrorActionPreference = 'Stop'

$root = Split-Path $PSScriptRoot -Parent
$introPath = Join-Path $root 'assets/locations/intro.location'
$nextPath = Join-Path $root 'assets/locations/test2.location'
$mainPath = Join-Path $root 'main.cpp'

function Read-Location($path) {
	if (-not (Test-Path $path)) {
		throw "Location file not found: $path"
	}

	$values = @{}
	foreach ($line in Get-Content $path) {
		$text = $line.Trim()
		if ($text.Length -eq 0 -or $text.StartsWith('#')) { continue }
		$parts = $text.Split('=', 2)
		if ($parts.Count -ne 2) { throw "Invalid location line in $path`: $text" }
		$values[$parts[0].Trim()] = $parts[1].Trim()
	}
	return $values
}

function Assert-Equal($actual, $expected, $name) {
	if ($actual -ne $expected) {
		throw "$name`: expected '$expected', got '$actual'"
	}
	Write-Host "PASS $name"
}

function Assert-File($relativePath) {
	$path = Join-Path $root $relativePath
	if (-not (Test-Path $path)) { throw "Required file not found: $relativePath" }
	Write-Host "PASS file $relativePath"
}

$intro = Read-Location $introPath
$next = Read-Location $nextPath

Assert-Equal $intro['id'] 'intro' 'intro id'
Assert-Equal $intro['transition.nextLocation'] 'test2' 'next location'
Assert-Equal $next['id'] 'test2' 'target location id'
Assert-File $intro['background.texture']
Assert-File $next['background.texture']

$testDelay = 10.0
$fadeDuration = [double]::Parse($intro['transition.duration'],
	[Globalization.CultureInfo]::InvariantCulture)
$elapsed = 0.0
$transitionStarted = $false
$transitionCompleted = $false

while ($elapsed -lt $testDelay) {
	$elapsed += 0.1
	if ($elapsed -ge $testDelay) {
		$transitionStarted = $true
	}
}

if (-not $transitionStarted) { throw 'Automatic transition did not start after 10 seconds' }
Write-Host 'PASS automatic transition starts after 10 seconds'

$fadeElapsed = 0.0
while ($fadeElapsed -lt $fadeDuration) {
	$fadeElapsed += 0.1
}
$transitionCompleted = $fadeElapsed -ge $fadeDuration
if (-not $transitionCompleted) { throw 'Fade transition did not complete' }
Write-Host 'PASS fade transition completes'
Assert-Equal $next['id'] 'test2' 'loaded location after fade'

$main = Get-Content $mainPath -Raw
if ($main -notmatch 'world\.load\(pendingLocation\)') { throw 'WorldManager transition call is missing' }
Write-Host 'PASS WorldManager transition call'
if ($main -notmatch 'transitionState = TransitionState::FadeOut') { throw 'Fade-out transition state is missing' }
Write-Host 'PASS fade-out state'

Write-Host ''
Write-Host 'Automatic location transition test is valid.' -ForegroundColor Green
