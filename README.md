![Tripal Dependency](https://img.shields.io/badge/tripal-%3E=3.0-brightgreen)
![Module is Generic](https://img.shields.io/badge/generic-confirmed-brightgreen)

[![Build Status](https://travis-ci.org/UofS-Pulse-Binfo/tripal_hq_imports.svg?branch=master)](https://travis-ci.org/UofS-Pulse-Binfo/tripal_hq_imports)
[![Test Coverage](https://api.codeclimate.com/v1/badges/598206b15a4410687d38/test_coverage)](https://codeclimate.com/github/UofS-Pulse-Binfo/tripal_hq_imports/test_coverage)

# Tripal HQ Imports

Tripal HQ provides a user-contributed content control center and administrative toolbox for your Tripal site. Tripal HQ Imports extends [Tripal HQ](https://github.com/statonlab/tripal_hq) to support TripalImporters. Specifically, this allows users to submit Tripal Importers, administrators to review the submission and data is only inserted into Chado once the administrator approves the submission. 

## UNDER DEVELOPMENT

This module is currently under active development. **It is not ready for use.** If you are interested in this module, please star it so we know there is need. Thank you!

## Current Features

Only current features are listed below. This does not reflect all features which will be available once development is complete but is meant to provide you with an idea of development progress.

### Adds Data Import support to Tripal HQ user Dashboard

Users with permission to submit data through Tripal HQ, now have access to an "Import data file" action link on their dashboard. Once clicked, users are presented with the full list of Tripal Importers (including custom ones) which allows them to pick their data file type, enter the required metadata and upload the file. At this point the submission is put into a holding area waiting for administrator approval.

![User Dashboard Screenshot](https://user-images.githubusercontent.com/1566301/66760602-afc8ff80-ee5f-11e9-9934-b8c065573f83.png).

Their submissions will be summarized on the same dashboard as their Tripal Content for a unified experience!

### Administration
- Data file import forms are created automatically based on the TripalImporter::form() and TripalImporter:validate() is run on submission to ensure meta data matches standards.
