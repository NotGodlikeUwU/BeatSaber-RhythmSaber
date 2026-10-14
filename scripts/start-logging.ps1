param([string]$Serial,[string]$Output='rhythmsaber-logcat.txt')
$ErrorActionPreference='Stop'
$taskArgs=@(); if($Serial) { $taskArgs+=@('-s',$Serial) }
$taskArgs+=@('logcat','-v','threadtime')
& adb @taskArgs | Select-String 'rhythmsaber|RhythmSaber|AndroidRuntime|crash|Scotland2' | Tee-Object -FilePath $Output
