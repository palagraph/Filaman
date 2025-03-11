# Changelog

## [1.4.1] - 2025-03-10
### Added
- added new .step, now with correct individual parts
- added changelog
- Add files via upload
- added .stp files of modifications
- added merged picture
- added pictures of components bought from AliE
- Add files via upload
- added pictures for heat insert location
- added pictures showing heat insert location
- remove unnecessary delay in MQTT setup and add delay before restart
- add new 3D print file for Filaman scale
- added Discord Server

### Changed
- update platformio.ini for version v1.4.1
- refactor length calculation to convert total length to meters before formatting
- Merge pull request #16 from spitzbirne32/main
- improved housing to show display better
- removed CAD, as they were all duplicates
- typo in AliE link
- Delete usermod/spitzbirne32/STL/README.md
- Update README.md
- moved pictures of parts into dedicated folders
- Update README.md
- Update README.md
- Update README.md
- Delete usermod/spitzbirne32/STL/ScaleTop_Heatinsert_Location_usermod_spitzbirne32_.png
- Delete usermod/spitzbirne32/STL/Housing_Heatinsert_Location_usermod_spitzbirne32_.png
- created folders
- Update README.md
- Update README.md
- Create README.md
- Update README.md
- Update README.md
- Create README.md
- Merge pull request #15 from ManuelW77/main
- Merge pull request #14 from janecker/scale-calibration-rework
- Reworks the scale calibration handling
- remove redundant scale calibration checks and enhance task management
- enhance AMS data handling and streamline spool auto-setting logic
- adjust stack size and improve scale calibration logic
- update labels and input types for better clarity and functionality
- update documentation for clarity and accuracy

### Fixed
- correct typo in console log for total length


## [1.4.0] - 2025-03-01
### Added
- add support for Spoolman Octoprint Plugin in README files
- add OctoPrint integration with configurable fields and update functionality
- add version comparison function and check for outdated versions before updates
- remove unused version and protocol fields from JSON output; add error message for insufficient memory

### Changed
- update NFC tag references to include NTAG213 and clarify storage capacity
- bump version to 1.4.0
- remove unused version and protocol fields from NFC data packet
- sort vendors alphabetically in the dropdown list
- Merge pull request #10 from janecker/nfc-improvements
- Improves NFC Tag handling


## [1.3.99] - 2025-02-28
### Changed
- update platformio.ini for version v1.3.99
- update workflows to build firmware with LittleFS instead of SPIFFS


## [1.3.98] - 2025-02-28
### Changed
- update platformio.ini for version v1.3.98
- migrate from SPIFFS to LittleFS for file handling
- remove unused VSCode settings file
- remove commented-out spoolman and filaman data from api.cpp


## [1.3.97] - 2025-02-28
### Added
- füge Bestätigungsmeldung für Spool-Einstellung hinzu
- verbessere WLAN-Konfiguration und füge mDNS-Unterstützung hinzu
- aktualisiere OLED-Anzeige mit Versionsnummer und verbessere Textausrichtung
- füge regelmäßige WLAN-Verbindungsüberprüfung hinzu
- aktualisiere Schaltplan-Bild
- zeige Versionsnummer im OLED-Display an

### Changed
- update platformio.ini for version v1.3.97
- entferne text-shadow von deaktivierten Schaltflächen
- füge Link zum Wiki für detaillierte Informationen über die Nutzung hinzu

### Fixed
- Speichernutzung optimiert
- behebe doppelte http.end() Aufrufe in checkSpoolmanExtraFields
- optimiere Verzögerungen und Stackgrößen in NFC-Task-Funktionen
- entferne ungenutzte Bibliotheken und Debug-Ausgaben aus main.cpp


## [1.3.96] - 2025-02-25
### Added
- füge Unterstützung für Spoolman-Einstellungen hinzu und aktualisiere die Benutzeroberfläche
- entferne die sendAmsData-Funktion aus der API-Schnittstelle
- erweitere Bambu-Credentials um AutoSend-Zeit und aktualisiere die Benutzeroberfläche
- erweitere Bambu-Credentials mit AutoSend-Wartezeit und aktualisiere die Benutzeroberfläche
- add espRestart function and replace delay with vTaskDelay for OTA update process
- implement OTA update functionality with backup and restore for configurations
- add own_filaments.json and integrate custom filament loading in bambu.cpp

### Changed
- update platformio.ini for version v1.3.96

### Fixed
- aktualisiere Bedingungen für die AMS-Datenaktualisierung und entferne unnötige Aufrufe
- aktualisiere Bedingung für den Fortschritt der OTA-Update-Nachricht
- update auto set logic to check RFID tag before setting Bambu spool


## [1.3.95] - 2025-02-24
### Changed
- update webpages for version v1.3.95

### Fixed
- bind autoSendToBambu variable to checkbox in spoolman.html


## [1.3.94] - 2025-02-24
### Changed
- update webpages for version v1.3.94

### Fixed
- correct payload type check in NFC write event handling


## [1.3.93] - 2025-02-24
### Added
- implement auto send feature for Bambu spool management and update related configurations
- add debug mode instructions for Spoolman in README
- add wiring diagrams to README for PN532 I2C setup

### Changed
- update webpages for version v1.3.93
- simplify filament names in JSON configuration
- update findFilamentIdx to return structured result and improve type searching logic
- update README to reflect PN532 I2C configuration and remove SPI pin details

### Fixed
- remove debug output from splitTextIntoLines and update weight display logic in scanRfidTask
- enhance weight display logic for negative values
- remove unnecessary CPU frequency configuration from setup function


## [1.3.92] - 2025-02-24
### Changed
- update webpages for version v1.3.92
- remove commented-out code in setBambuSpool function
- update installation instructions and formatting in README files

### Fixed
- configure CPU frequency settings in setup function only for testing
- update comment to clarify NVS reading process
- adjust weight display logic to handle cases for weight less than 2
- update weight display logic to handle negative and specific weight cases


## [1.3.91] - 2025-02-23
### Added
- update GitHub Actions workflow for FTP firmware upload with improved credential checks

### Changed
- update webpages for version v1.3.91


## [1.3.90] - 2025-02-23
### Added
- update index.html for improved content structure and additional links
- improve UI for Spoolman and Bambu Lab printer credentials, enhancing layout and styling
- update README files with HSPI default PINs and add ESP32 pin diagram
- implement scale calibration checks and update start_scale function to return calibration status
- add FTP upload functionality to GitHub release workflow and update installation instructions in README

### Changed
- update webpages for version v1.3.90

### Fixed
- remove debug secrets check from Gitea release workflow


## [1.3.89] - 2025-02-23
### Changed
- update webpages for version v1.3.89

### Fixed
- update Gitea release workflow to use vars for FTP credentials


## [1.3.88] - 2025-02-23
### Changed
- update webpages for version v1.3.88

### Fixed
- update Gitea release workflow to use secrets for FTP credentials


## [1.3.87] - 2025-02-23
### Changed
- update webpages for version v1.3.87

### Fixed
- enhance FTP upload workflow with credential checks and version output


## [1.3.86] - 2025-02-23
### Changed
- update webpages for version v1.3.86

### Fixed
- streamline FTP credentials usage in Gitea release workflow


## [1.3.85] - 2025-02-23
### Added
- add FTP_USER variable for Gitea release workflow

### Changed
- update webpages for version v1.3.85


## [1.3.84] - 2025-02-23
### Added
- add FTP_HOST variable for firmware upload in Gitea release workflow

### Changed
- update webpages for version v1.3.84


## [1.3.83] - 2025-02-23
### Changed
- update webpages for version v1.3.83

### Fixed
- correct variable interpolation for FTP credentials in Gitea release workflow


## [1.3.82] - 2025-02-23
### Added
- update Gitea release workflow to use variable interpolation for FTP credentials

### Changed
- update webpages for version v1.3.82


## [1.3.81] - 2025-02-23
### Added
- update Gitea release workflow to use environment variables for FTP credentials and version

### Changed
- update webpages for version v1.3.81


## [1.3.80] - 2025-02-23
### Added
- add FTP_USER and FTP_PASSWORD secrets for firmware upload in Gitea release workflow

### Changed
- update webpages for version v1.3.80


## [1.3.79] - 2025-02-23
### Added
- add FTP_USER input for firmware upload in Gitea release workflow

### Changed
- update webpages for version v1.3.79


## [1.3.78] - 2025-02-23
### Changed
- update webpages for version v1.3.78

### Fixed
- change FTP protocol from FTPS to FTP for file upload in workflow


## [1.3.77] - 2025-02-23
### Changed
- update webpages for version v1.3.77

### Fixed
- replace ncftp with lftp for secure firmware upload


## [1.3.76] - 2025-02-23
### Changed
- update webpages for version v1.3.76

### Fixed
- replace FTP action with curl for secure firmware upload and install ncftp


## [1.3.75] - 2025-02-23
### Changed
- update webpages for version v1.3.75

### Fixed
- update FTP user and enhance SSL options in gitea-release workflow


## [1.3.74] - 2025-02-23
### Changed
- update webpages for version v1.3.74

### Fixed
- update password syntax in gitea-release workflow


## [1.3.73] - 2025-02-23
### Changed
- update webpages for version v1.3.73
- update version to 1.3.72 in platformio.ini


## [1.3.72] - 2025-02-23
### Changed
- update webpages for version v1.3.72

### Fixed
- update FTP options for Gitea release workflow


## [1.3.71] - 2025-02-23
### Added
- add FTP upload step for firmware in Gitea release workflow

### Changed
- update webpages for version v1.3.71


## [1.3.70] - 2025-02-23
### Changed
- update webpages for version v1.3.70


## [1.3.69] - 2025-02-23
### Changed
- update webpages for version v1.3.69

### Fixed
- update release note generation to use the second latest tag


## [1.3.68] - 2025-02-23
### Changed
- update webpages for version v1.3.68

### Fixed
- update release note generation to include commit hash and author
- remove commented test line from platformio.ini


## [1.3.67] - 2025-02-23
### Changed
- update webpages for version v1.3.67
- ci: update release note generation to use the latest tag


## [1.3.66] - 2025-02-23
### Changed
- update webpages for version v1.3.66
- ci: remove redundant git fetch for tags in release note generation


## [1.3.65] - 2025-02-22
### Changed
- update webpages for version v1.3.65
- ci: improve release note generation by fetching tags and sorting unique commits


## [1.3.64] - 2025-02-22
### Changed
- update webpages for version v1.3.64
- remove unnecessary closing tags from header.html


## [1.3.63] - 2025-02-22
### Added
- update update-form background and add glass border effect

### Changed
- update webpages for version v1.3.63
- update release note generation for initial release handling


## [1.3.62] - 2025-02-22
### Changed
- update webpages for version v1.3.62
- update background colors and improve layout for update sections


## [1.3.61] - 2025-02-22
### Added
- update release notes generation to use previous tag for changes

### Changed
- update webpages for version v1.3.61


## [1.3.60] - 2025-02-22
### Added
- remove automatic git push from changelog update script
- implement release notes generation with categorized changes since last tag

### Changed
- update webpages for version v1.3.60


## [1.3.59] - 2025-02-22
### Added
- implement enhanced update progress handling and WebSocket notifications
- improve update progress reporting and enhance WebSocket notifications
- enhance update progress handling and add WebSocket closure notification
- implement WebSocket for update progress and enhance update response handling

### Changed
- update webpages for version v1.3.59


## [1.3.58] - 2025-02-22
### Added
- implement backup and restore functionality for Bambu credentials and Spoolman URL

### Changed
- update webpages for version v1.3.58
- update upgrade page message and improve progress display logic


## [1.3.57] - 2025-02-22
### Changed
- update webpages for version v1.3.57
- update header title to 'Filament Management Tool' in multiple HTML files


## [1.3.56] - 2025-02-22
### Changed
- update webpages for version v1.3.56
- update header title and improve SPIFFS update error handling
- clarify comments in Gitea and GitHub release workflows


## [1.3.55] - 2025-02-22
### Changed
- update webpages for version v1.3.55
- update component descriptions in README files


## [1.3.54] - 2025-02-22
### Changed
- update webpages for version v1.3.54
- workflow: update SPIFFS binary creation to exclude header


## [1.3.53] - 2025-02-22
### Changed
- version: update to version 1.3.53
- update changelog for version 1.3.51
- update changelog for version 1.3.51
- workflow: update SPIFFS binary magic byte and revert version to 1.3.51


## [1.3.52] - 2025-02-22
### Changed
- update webpages for version v1.3.52
- workflow: update SPIFFS binary creation to use correct chip revision (0xEB for Rev 3)


## [1.3.51] - 2025-02-22
### Changed
- update changelog for version 1.3.51
- workflow: update SPIFFS binary magic byte and revert version to 1.3.51

## [1.3.50] - 2025-02-22
### Changed
- update webpages for version v1.3.50


## [1.3.49] - 2025-02-22
### Changed
- update webpages for version v1.3.49
- workflow: update SPIFFS binary header to use correct chip revision


## [1.3.48] - 2025-02-22
### Changed
- update webpages for version v1.3.48
- workflow: update SPIFFS binary header for firmware release


## [1.3.47] - 2025-02-22
### Changed
- update webpages for version v1.3.47
- workflow: optimize firmware and SPIFFS update process, improve progress handling and logging


## [1.3.46] - 2025-02-22
### Changed
- update webpages for version v1.3.46


## [1.3.45] - 2025-02-22
### Changed
- update webpages for version v1.3.45
- workflow: update SPIFFS binary creation to include minimal header and adjust update validation logic


## [1.3.44] - 2025-02-22
### Changed
- update webpages for version v1.3.44
- update header title to 'Hollo Lollo Trollo'
- update header title to 'Filament Management Tool' and improve update response messages


## [1.3.43] - 2025-02-22
### Changed
- update webpages for version v1.3.43
- update header title to 'Hollo Lollo Trollo'


## [1.3.42] - 2025-02-22
### Changed
- update webpages for version v1.3.42

### Fixed
- correct path for SPIFFS binary creation in Gitea release workflow


## [1.3.41] - 2025-02-22
### Changed
- update webpages for version v1.3.41

### Fixed
- remove redundant buffer size setting in NFC initialization
- update SPIFFS binary creation and enhance NFC buffer size


## [1.3.40] - 2025-02-22
### Changed
- update webpages for version v1.3.40

### Fixed
- update SPIFFS binary header and enhance WebSocket error handling


## [1.3.39] - 2025-02-22
### Changed
- update webpages for version v1.3.39
- workflow: update SPIFFS binary creation to set chip version to max supported


## [1.3.38] - 2025-02-22
### Changed
- update webpages for version v1.3.38
- workflow: update SPIFFS binary creation with minimal ESP32 image header


## [1.3.37] - 2025-02-22
### Changed
- update webpages for version v1.3.37
- workflow: update ESP32-WROOM image header for SPIFFS binary creation


## [1.3.36] - 2025-02-22
### Changed
- update webpages for version v1.3.36
- partition: update SPIFFS binary header and offsets in workflow files


## [1.3.35] - 2025-02-22
### Changed
- update webpages for version v1.3.35
- partition: update SPIFFS binary header and offsets in workflow files


## [1.3.34] - 2025-02-22
### Changed
- update webpages for version v1.3.34
- partition: update SPIFFS binary creation and offsets in workflow files


## [1.3.33] - 2025-02-22
### Changed
- update webpages for version v1.3.33
- partition: update spiffs offset and app sizes in partition files
- partition: update spiffs offset in partition files
- partition: update app sizes and offsets in partitions.csv


## [1.3.32] - 2025-02-22
### Changed
- update webpages for version v1.3.32
- workflow: update magic byte for SPIFFS binary creation


## [1.3.31] - 2025-02-22
### Changed
- update webpages for version v1.3.31
- workflow: remove unnecessary data and SPIFFS change checks from release workflows


## [1.3.30] - 2025-02-22
### Changed
- update webpages for version v1.3.30
- workflow: update Gitea and GitHub release workflows to create SPIFFS binary with magic byte


## [1.3.29] - 2025-02-21
### Changed
- update webpages for version v1.3.29
- workflow: update Gitea release workflow to create release before file uploads


## [1.3.28] - 2025-02-21
### Changed
- update webpages for version v1.3.28
- workflow: update Gitea release workflow to use file uploads with curl


## [1.3.27] - 2025-02-21
### Added
- workflow: add GITEA_TOKEN secret for Gitea API access in release workflows

### Changed
- update webpages for version v1.3.27


## [1.3.26] - 2025-02-21
### Changed
- update webpages for version v1.3.26

### Fixed
- workflow: improve Gitea release workflow with enhanced error handling and debug outputs


## [1.3.25] - 2025-02-21
### Changed
- update webpages for version v1.3.25
- workflow: update Gitea release workflow to include RUNNER_NAME and improve error handling


## [1.3.24] - 2025-02-21
### Changed
- update webpages for version v1.3.24
- workflow: rename update files to upgrade in GitHub release workflow
- workflow: aktualisiere bestehende Einträge im Changelog für vorhandene Versionen

### Fixed
- workflow: improve Gitea release process with dynamic URL determination and debug outputs


## [1.3.23] - 2025-02-21
### Changed
- update webpages for version v1.3.23

### Fixed
- workflow: enhance Gitea release process with debug outputs and API connection checks


## [1.3.22] - 2025-02-21
### Added
- workflow: improve Gitea release process with additional environment variables and error handling

### Changed
- update webpages for version v1.3.22


## [1.3.21] - 2025-02-21
### Changed
- update webpages for version v1.3.21
- workflow: enhance Gitea release process with API integration and token management


## [1.3.20] - 2025-02-21
### Changed
- update webpages for version v1.3.20
- workflow: enable git tagging and pushing for Gitea releases


## [1.3.19] - 2025-02-21
### Changed
- update webpages for version v1.3.19
- workflow: enable git push for version tagging in Gitea release


## [1.3.18] - 2025-02-21
### Changed
- ACHTUNG: Installiere einmal das filaman_full.bin danach kannst du über die upgrade Files aktualisieren und deine Settings bleiben auch erhalten.
- ATTENTION: Install the filaman_full.bin once, then you can update via the upgrade files and your settings will also be retained.



## [1.3.18] - 2025-02-21
### Added
- add note about filaman_full.bin installation in changelog

### Changed
- update webpages for version v1.3.18
- update changelog for version 1.3.18 and enhance update script for existing entries

## [1.3.17] - 2025-02-21
### Changed
- update webpages for version v1.3.17
- ci: comment out git tag and push commands in gitea-release workflow


## [1.3.16] - 2025-02-21
### Changed
- update webpages for version v1.3.16
- ci: update filenames for firmware and website binaries in release workflows


## [1.3.15] - 2025-02-21
### Changed
- update webpages for version v1.3.15

### Fixed
- ci: fix missing 'fi' in GitHub release workflow script


## [1.3.14] - 2025-02-21
### Changed
- update webpages for version v1.3.14
- ci: update GitHub release workflow to improve file upload handling


## [1.3.13] - 2025-02-21
### Changed
- update webpages for version v1.3.13
- ci: update GitHub release workflow to use RELEASE_TOKEN for improved security


## [1.3.12] - 2025-02-21
### Changed
- update webpages for version v1.3.12
- ci: enhance GitHub release workflow with token handling and file upload improvements


## [1.3.11] - 2025-02-21
### Changed
- update webpages for version v1.3.11
- ci: refactor Gitea release workflow by simplifying input handling and removing unnecessary checks


## [1.3.10] - 2025-02-21
### Changed
- update webpages for version v1.3.10
- ci: simplify GitHub release workflow by removing provider verification step


## [1.3.9] - 2025-02-21
### Changed
- update webpages for version v1.3.9
- ci: comment out permissions for GitHub release workflow


## [1.3.8] - 2025-02-21
### Added
- add Gitea and GitHub release workflows

### Changed
- update webpages for version v1.3.8


## [1.3.7] - 2025-02-21
### Added
- add GitHub and Gitea release workflows

### Changed
- update webpages for version v1.3.7


## [1.3.6] - 2025-02-21
### Changed
- update webpages for version v1.3.6

### Fixed
- update GitHub token reference and correct file path in release workflow


## [1.3.5] - 2025-02-21
### Added
- enhance release workflow to support Gitea alongside GitHub

### Changed
- update webpages for version v1.3.5


## [1.3.4] - 2025-02-21
### Added
- add Gitea and GitHub release workflows

### Changed
- update webpages for version v1.3.4
- Merge branch 'old'


## [1.3.3] - 2025-02-21
### Changed
- update webpages for version v1.3.3

### Fixed
- correct directory path in GitHub workflows for SPIFFS binary


## [1.3.2] - 2025-02-21
### Added
- add missing conditional exit in release workflow

### Changed
- update webpages for version v1.3.2


## [1.3.1] - 2025-02-21
### Added
- enhance GitHub and Gitea release workflows with Python setup and binary preparation

### Changed
- update webpages for version v1.3.1


## [1.3.0] - 2025-02-21
### Changed
- bump version to 1.3.0 in platformio.ini


## [1.2.102] - 2025-02-21
### Changed
- update webpages for version v1.2.102

### Fixed
- adjust bootloader offset in binary merge for Gitea and GitHub workflows


## [1.2.101] - 2025-02-21
### Changed
- update webpages for version v1.2.101
- always create SPIFFS binary in release workflows
- migrate calibration value storage from EEPROM to NVS


## [1.2.100] - 2025-02-21
### Changed
- update webpages for version v1.2.100
- remove OTA handling and JSON backup/restore functions


## [1.2.99] - 2025-02-21
### Added
- add SPIFFS change detection and binary copying to release workflows
- add backup and restore functions for JSON configurations during OTA updates

### Changed
- update webpages for version v1.2.99
- update JSON field type checks from JsonObject to String for improved validation
- update JSON handling in API and Bambu modules for improved object management
- update platformio.ini dependencies and improve version handling in website.cpp
- update Cache-Control header to reflect a 1-week duration
- remove version definition from website.cpp
- optimize WiFi and WebSocket settings; enhance TCP/IP stack configuration
- update upgrade page title and heading; adjust cache control duration


## [1.2.98] - 2025-02-21
### Changed
- update webpages for version v1.2.98


## [1.2.97] - 2025-02-21
### Changed
- update webpages for version v1.2.97
- streamline Gitea and GitHub release workflows to check for data changes and update binary handling


## [1.2.96] - 2025-02-21
### Added
- add SPIFFS build step to Gitea and GitHub release workflows

### Changed
- update webpages for version v1.2.96


## [1.2.95] - 2025-02-21
### Added
- enhance update process with separate forms for firmware and webpage uploads, including validation and improved UI
- add API endpoint for version retrieval and update HTML to display dynamic version

### Changed
- update webpages for version v1.2.95
- bump version to 1.2.94 in platformio.ini


## [1.2.91] - 2025-02-20
### Added
- add file existence checks before uploading binaries in release workflows

### Changed
- update webpages for version v1.2.91


## [1.2.90] - 2025-02-20
### Changed
- update webpages for version v1.2.90
- update Gitea and GitHub release workflows to include SPIFFS directory creation and firmware copying


## [1.2.89] - 2025-02-20
### Changed
- update webpages for version v1.2.89
- enhance OTA update process with SPIFFS support and improved error handling


## [1.2.88] - 2025-02-20
### Changed
- update webpages for version v1.2.88

### Fixed
- improve OTA update handling and logging for better error reporting


## [1.2.87] - 2025-02-20
### Changed
- update webpages for version v1.2.87

### Fixed
- update firmware build process and remove unused OTA environment


## [1.2.86] - 2025-02-20
### Changed
- update webpages for version v1.2.86

### Fixed
- update SPIFFS offset and size in release workflows and partitions.csv


## [1.2.85] - 2025-02-20
### Added
- ci: streamline release workflows by removing unnecessary binary uploads and adding SPIFFS formatting

### Changed
- update webpages for version v1.2.85


## [1.2.84] - 2025-02-20
### Changed
- update webpages for version v1.2.84
- ci: update installation steps for xxd to include package list update


## [1.2.83] - 2025-02-20
### Changed
- update webpages for version v1.2.83
- replace hexdump with xxd for magic byte verification in release workflows


## [1.2.82] - 2025-02-20
### Changed
- update webpages for version v1.2.82
- enhance Gitea and GitHub release workflows to include magic byte handling and improve binary verification


## [1.2.81] - 2025-02-20
### Changed
- update webpages for version v1.2.81
- update Gitea and GitHub release workflows to use esp32dev_ota for building and uploading firmware and SPIFFS binaries


## [1.2.80] - 2025-02-20
### Added
- update Gitea and GitHub release workflows to include new firmware and SPIFFS binaries; fix version display in HTML files to v1.2.78

### Changed
- update webpages for version v1.2.80
- bump version to 1.2.79 in platformio.ini


## [1.2.79] - 2025-02-20
### Changed
- update webpages for version v1.2.79
- simplify Gitea release workflow by using esptool for binary creation


## [1.2.78] - 2025-02-20
### Changed
- update webpages for version v1.2.78
- streamline Gitea release workflow and update version to 1.2.77

### Fixed
- update Gitea and GitHub release workflows to improve binary preparation and verification fix: correct version number in HTML files and platformio.ini to v1.2.76 enhance: streamline OTA update handling by removing unnecessary magic byte checks


## [1.2.77] - 2025-02-20
### Changed
- optimize Gitea release workflow by simplifying build steps and improving file handling
- update webpages for version v1.2.77


## [1.2.76] - 2025-02-20
### Added
- enhance OTA update process with improved file verification and new OTA data initialization

### Changed
- update webpages for version v1.2.76
- streamline Gitea release workflow and remove obsolete OTA data initialization script


## [1.2.75] - 2025-02-20
### Changed
- update webpages for version v1.2.75
- simplify OTA update handling by removing unnecessary variables and improving error reporting


## [1.2.74] - 2025-02-20
### Changed
- update webpages for version v1.2.74
- enhance OTA update process with improved handling of full image updates and SPIFFS data


## [1.2.73] - 2025-02-20
### Changed
- update webpages for version v1.2.73
- improve OTA update process with enhanced size checks and progress logging

### Fixed
- enhance OTA update process with improved size checks and debugging output


## [1.2.72] - 2025-02-20
### Changed
- update webpages for version v1.2.72
- simplify OTA update process by removing unnecessary buffer and adjusting offsets


## [1.2.71] - 2025-02-20
### Added
- update version number to v1.2.65 in HTML files and platformio.ini; add script to create full binary

### Changed
- update webpages for version v1.2.71


## [1.2.70] - 2025-02-20
### Added
- add logging to stopAllTasks for better debugging

### Changed
- update webpages for version v1.2.70


## [1.2.69] - 2025-02-20
### Changed
- update webpages for version v1.2.69


## [1.2.68] - 2025-02-20
### Added
- add task handles for BambuMqttTask and ScaleTask; rename stopTasks to stopAllTasks

### Changed
- update webpages for version v1.2.68

### Fixed
- update stopAllTasks to suspend RfidReaderTask instead of NfcTask


## [1.2.67] - 2025-02-20
### Added
- improve OTA upload handling and add SPIFFS update support

### Changed
- update webpages for version v1.2.67


## [1.2.66] - 2025-02-20
### Added
- enhance OTA upload to support SPIFFS updates

### Changed
- update webpages for version v1.2.66


## [1.2.65] - 2025-02-20
### Changed
- update webpages for version v1.2.65


## [1.2.64] - 2025-02-20
### Changed
- update webpages for version v1.2.64

### Fixed
- improve error handling in OTA upload process


## [1.2.63] - 2025-02-20
### Added
- update version to 1.2.63

### Changed
- update webpages for version v1.2.63


## [1.2.62] - 2025-02-20
### Added
- update version to 1.2.62

### Changed
- update webpages for version 1.2.62


## [1.2.61] - 2025-02-20
### Added
- update version to 1.2.61 and enhance OTA update error handling

### Changed
- update webpages for version 1.2.61


## [1.2.60] - 2025-02-20
### Added
- update version to 1.2.60 in platformio configuration

### Changed
- update webpages for version 1.2.60


## [1.2.59] - 2025-02-20
### Added
- update version to 1.2.59 and enhance OTA upload handling

### Changed
- update webpages for version 1.2.59


## [1.2.58] - 2025-02-20
### Added
- update version to 1.2.58 in HTML files and platformio configuration


## [1.2.57] - 2025-02-20
### Added
- update version to 1.2.57 in platformio configuration

### Fixed
- update spiffs offset and sizes in firmware and partitions configuration


## [1.2.56] - 2025-02-19
### Added
- update version to 1.2.56 and adjust firmware and partition settings

### Changed
- remove redundant comment from Gitea release workflow


## [1.2.55] - 2025-02-19
### Added
- update version to 1.2.55 and adjust Gitea release workflow to remove 'v' prefix from version


## [1.2.54] - 2025-02-19
### Added
- update version to 1.2.54 and add check for missing changelog in Gitea release workflow


## [1.2.53] - 2025-02-19
### Added
- update version to 1.2.53 and enhance Gitea release workflow to handle missing changelog


## [1.2.52] - 2025-02-19
### Added
- update version to 1.2.52 and improve Gitea release workflow output handling


## [1.2.51] - 2025-02-19
### Added
- update version to 1.2.51 and add debug output in Gitea release workflow


## [1.2.50] - 2025-02-19
### Added
- update version to 1.2.50 and remove obsolete build workflow


## [1.2.49] - 2025-02-19
### Added
- update version to 1.2.49 and modify changelog reading in Gitea release workflow


## [1.2.48] - 2025-02-19
### Added
- update version to 1.2.48 and modify changelog reading in release workflows


## [1.2.47] - 2025-02-19
### Added
- update version to 1.2.47 and modify changelog reading in GitHub release workflow


## [1.2.46] - 2025-02-19
### Added
- update Gitea release workflow to include changelog and increment version to 1.2.46


## [1.2.45] - 2025-02-19
### Added
- update Gitea release workflow to use dynamic upload URL and increment version to 1.2.45


## [1.2.44] - 2025-02-19
### Added
- update Gitea release workflow to include additional outputs and increment version to 1.2.44


## [1.2.43] - 2025-02-19
### Added
- update Gitea release workflow by removing unnecessary outputs and increment version to 1.2.43


## [1.2.42] - 2025-02-19
### Added
- update Gitea release workflow to include additional outputs and increment version to 1.2.42


## [1.2.41] - 2025-02-19
### Added
- update Gitea release workflow debug output and increment version to 1.2.41


## [1.2.40] - 2025-02-19
### Added
- add inputs for Gitea release workflow and increment version to 1.2.40


## [1.2.39] - 2025-02-19
### Added
- update Gitea release workflow to use Python virtual environment and increment version to 1.2.39


## [1.2.38] - 2025-02-19
### Added
- update Gitea release workflow and increment version to 1.2.38


## [1.2.37] - 2025-02-19
### Added
- update Gitea release workflow and increment version to 1.2.37


## [1.2.36] - 2025-02-19
### Added
- update Gitea release workflow and increment version to 1.2.36


## [1.2.35] - 2025-02-19
### Added
- update Gitea release workflow and version to 1.2.35


## [1.2.34] - 2025-02-19
### Added
- update version to 1.2.34 and enhance Gitea and GitHub release workflows


## [1.2.33] - 2025-02-19
### Added
- update version to 1.2.33 and refactor release workflows for Gitea and GitHub


## [1.2.32] - 2025-02-19
### Added
- update version to 1.2.32 and adjust workflow dependencies for GitHub and Gitea releases


## [1.2.31] - 2025-02-19
### Added
- update version to 1.2.31


## [1.2.30] - 2025-02-19
### Added
- update version to 1.2.30 and enhance release workflows for Gitea and GitHub with improved artifact handling and changelog integration


## [1.2.29] - 2025-02-19
### Added
- update version to 1.2.29 and enhance Gitea release workflow with improved error handling and debugging


## [1.2.28] - 2025-02-19
### Added
- add build and release workflows for Gitea and GitHub, increment version to 1.2.28


## [1.2.27] - 2025-02-19
### Added
- enhance CI workflows with provider detection and update version to 1.2.27


## [1.2.26] - 2025-02-19
### Added
- update release workflows for Gitea and GitHub, increment version to 1.2.26


## [1.2.25] - 2025-02-19
### Added
- update GitHub release workflows and increment version to 1.2.25


## [1.2.24] - 2025-02-19
### Added
- update GitHub release workflow and increment version to 1.2.24


## [1.2.23] - 2025-02-19
### Added
- update Gitea release workflow and increment version to 1.2.23


## [1.2.22] - 2025-02-19
### Added
- update Gitea runner configuration and increment version to 1.2.22


## [1.2.21] - 2025-02-19
### Added
- update Gitea release condition and increment version to 1.2.21


## [1.2.20] - 2025-02-19
### Added
- update release workflows for GitHub and Gitea, increment version to 1.2.20


## [1.2.19] - 2025-02-19
### Added
- add Gitea and GitHub release workflows for version 1.2.19


## [1.2.18] - 2025-02-19
### Added
- remove Gitea and GitHub release workflows and increment version to 1.2.18


## [1.2.17] - 2025-02-19
### Added
- update Gitea release workflow for version 1.2.17 and improve job structure


## [1.2.16] - 2025-02-19
### Added
- update Gitea release workflow and increment version to 1.2.16


## [1.2.15] - 2025-02-19
### Added
- update version to 1.2.15 and clean up Gitea release workflow


## [1.2.14] - 2025-02-19
### Added
- update version to 1.2.14 and refactor release workflows for improved GitHub and Gitea support


## [1.2.13] - 2025-02-19
### Added
- update version to 1.2.13 and enhance release workflow with Python setup and Gitea support


## [1.2.12] - 2025-02-19
### Added
- update version to 1.2.12 and refactor release workflows for improved provider detection and execution


## [1.2.11] - 2025-02-18
### Added
- update version to 1.2.11 and enhance GitHub and Gitea release workflows with input validation and improved error handling


## [1.2.10] - 2025-02-18
### Added
- enhance Gitea release workflow with API connection verification and URL validation; update version to 1.2.10


## [1.2.9] - 2025-02-18
### Added
- update version to 1.2.9 in platformio.ini
- refactor GitHub and Gitea release workflows for improved version handling and firmware uploads


## [1.2.8] - 2025-02-18
### Added
- update version to 1.2.8 in platformio.ini; refactor Gitea and GitHub release workflows
- update version to 1.2.7 in platformio.ini; adjust Gitea release workflow


## [1.2.6] - 2025-02-18
### Added
- update version to 1.2.6 in platformio.ini
- update version to 1.2.5; enhance Gitea release workflow and streamline release process

### Changed
- update changelog for version 1.2.5


## [1.2.5] - 2025-02-18
### Added
- update version to 1.2.5; enhance Gitea release workflow and streamline release process


## [1.2.4] - 2025-02-18
### Added
- update version to 1.2.4 in HTML files and platformio.ini
- add GitHub and Gitea release workflows; streamline firmware release process


## [1.2.3] - 2025-02-18
### Added
- update version to 1.2.3; modify HTML files to reflect new version; enhance firmware update process and UI improvements


## [1.2.2] - 2025-02-18
### Added
- update version to 1.2.2; change OTA upgrade link in HTML files; enhance OTA upload handling with progress updates and JSON responses
- implement OTA update functionality with web interface; update partition settings and build configuration
- remove unused OTA server setup and related includes; update platformio.ini dependencies
- adjust weight counter threshold and optimize delay in RFID scanning; include scale header in NFC module
- update version to v1.2.1 and change upgrade link to OTA in HTML files; modify updateSpoolTagId function to return boolean


## [1.2.0] - 2025-02-17
### Added
- implement OTA functionality and update build scripts; change upgrade link to OTA in HTML
- update version to v1.2.0 and modify build scripts in platformio.ini; remove unused includes in scale.cpp and website.cpp
- update version to v1.2.0 and add upgrade link in HTML files
- add esp_wifi.h and set maximum transmit power in WiFi initialization

### Changed
- update changelog for version 1.1.0
- clean up changelog and update script execution in platformio.ini
- update changelog for version 1.1.0

### Fixed
- update version number in header to v1.1.0
- correct version number in nav bar


## [1.1.0] - 2025-02-16
### Changed
- clean up changelog and update script execution in platformio.ini
- update changelog for version 1.1.0

### Fixed
- correct version number in nav bar


## [1.0.5] - 2025-02-16
### Added
- update version to 1.0.5 and enhance changelog update process with automatic git push
- update changelog script to categorize commits and handle new changelog creation
- improve changelog update script to handle absolute paths and create new changelog if missing


## [1.0.4] - 2025-02-16
### Added
- improve changelog update script to handle absolute paths and create new changelog if missing


## [1.0.3] - 2025-02-16
### Added
- 

### Changed
-

### Fixed
-

## [1.0.2] - 2025-02-16
### Added
- Feature 1
- Feature 2

### Changed
- Change 1
- Change 2

### Fixed
- Fix 1
- Fix 2