# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [v3.1](https://github.com/sigfox-tech-radio/sigfox-ep-rf-api-semtech-lr11xx/releases/tag/v3.1) - 01 Jul 2026

### Fixed

* Remove **extra warnings**.

### Changed

* Upgrade to **LR11xx driver v3.0.0**.
* Return **error by default** in weak functions template.

### Known limitations

* **LBT** not implemented.

## [v3.0](https://github.com/sigfox-tech-radio/sigfox-ep-rf-api-semtech-lr11xx/releases/tag/v3.0) - 22 Nov 2024

### Changed

* Upgrade to **sigfox-ep-lib v4.0**
* Use lr11xx_rf_api_obj target object to build lr11xx_rf_api library.
* Remove unifdef dependency in all cmake with linked target.
* Update lr11xx semtech driver to 2.5.0.
* Add **LR11XX_HW_API_config_t** structure in HW API open function with **radio configuration pointer** to manage multi-RC front-ends.

### Known limitations

* **LBT** not implemented.

## [v2.1](https://github.com/sigfox-tech-radio/sigfox-ep-rf-api-semtech-lr11xx/releases/tag/v2.1) - 30 May 2024

### Added

* Add weak attribute to functions templates.
* Add **SFX_UNUSED** macro to remove extra warnings.
* Add bidirectional flag on HW API RX on and RX off functions.

### Fixed

* Initialize local latency variables to 0.

### Removed

* Remove lr11xx_hw_api.c from **LR11XX_RF_API_SOURCES** in `CMakeLists.txt`.

### Known limitations

* **LBT** not implemented.

## [v2.0](https://github.com/sigfox-tech-radio/sigfox-ep-rf-api-semtech-lr11xx/releases/tag/v2.0) - 22 Mar 2024

### Changed

* BIDIRECTIONAL flag compilation issue
* Rename **smtc_dpbsk.c** to **smtc_dbpsk.c**.
* Rename `RCx` compilation flags into `RCx_ZONE` for **Microchip MCUs compatibility**.
* Update with 2.4.1 SWDR001 semtech drivers.

### Removed

* Remove **stmtc_sigfox_lr11xx.h** file.

### Added

* New methods in **LR11XX_HW_API** to configure output power board oscillator.

### Known limitations

* **LBT** not implemented.

## [v1.2](https://github.com/sigfox-tech-radio/sigfox-ep-rf-api-semtech-lr11xx/releases/tag/v1.2) - 09 Nov 2023

### Added

* Implement `RF_API_start_continuous_wave()` and `RF_API_stop_continuous_wave()` functions for **type approval addon**.

### Removed

* Remove `doc` folder since images are now hosted on the GitHub wiki.

### Known limitations

* **LBT** not implemented.

## [v1.1](https://github.com/sigfox-tech-radio/sigfox-ep-rf-api-semtech-lr11xx/releases/tag/v1.1) - 10 Aug 2023

### Added

* Add `LR11XX_HW_API_status_t` status to manage HW_API errors.

### Changed

* Rename all errors. 
* Remove `cmake/lr11xx.patch` file and update external project version in CMakeLists.  

### Fixed

* Init Error callback in `LR11XX_RF_API_open()`.
* Change call order of lr11xx driver function in `LR11XX_RF_API_init()`.
* Update latency and change `lr1110_ctx.backup_bit_rate_bps_patch` only in TX mode (`src/manuf/lr11xx_rf_api.c`).   
* Remove **unused variable** compiltation warning when VERBOSE flag is disabled (`src/manuf/lr11xx_rf_api.c`).

### Known limitations

* **LBT** not implemented.

## [v1.0](https://github.com/sigfox-tech-radio/sigfox-ep-rf-api-semtech-lr11xx/releases/tag/v1.0) - 29 Jun 2023

### General

* First version of the LR11XX RF API implementation example.

### Known limitations

* **LBT** not implemented.
