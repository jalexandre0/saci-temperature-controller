## [New Release]  2017-02-12

### Changed
- Bug Fix: Fixed a bug where heat and cool can switched on at same time. 

## [New Release]  2017-02-10

### Added
- Low pass filter for smooth sensor readings and ploting
- Possibilities to add a second DS18B20 ( fridgeRead() )
- Sensors are upadted after main loop (faster readings)
- Instructions for CLI compilation

### Changed
- MIT to GPL-V3 License change
- Plugin configuration (and send delays...) are now centralized in config.h
- Moved delays to main.cpp instead plugin files.

### Fixed
-Typos preventing compilation in Unix-Like Systems

## [fix_profile] 2016-12-16

### Fixed
- Fixed issue with ramp profile

### Changed
- Changed output for /status.json and serial
