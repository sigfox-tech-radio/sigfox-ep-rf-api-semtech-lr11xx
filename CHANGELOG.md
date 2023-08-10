# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

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
