# Tripal Headquarters: Imports

Tripal HQ provides a user-contributed content control center and administrative toolbox for your Tripal site. Tripal HQ Imports extends [Tripal HQ](https://github.com/statonlab/tripal_hq) to support TripalImporters. Specifically, this allows users to submit Tripal Importers, administrators to review the submission and data is only inserted into Chado once the administrator approves the submission.

## Features

 - Users submit data files using the existing Tripal Importer forms -no extra forms!
 - Users and administrators can access data file submissions on the same dashboard as Tripal HQ content submissions
 - All TripalImporters should be supported! (excludes multi-page forms; e.g AnalyzedPhenotypes)
 - You can specify which importers should be available to users through native Drupal Permissions.

## Usage

### Adds Data Import support to Tripal HQ User Dashboard

Users with permission to submit data through Tripal HQ, now have access to an "Import data file" action link on their dashboard. Once clicked, users are presented with the full list of Tripal Importers (including custom ones) which allows them to pick their data file type, enter the required metadata and upload the file. At this point the submission is put into a holding area waiting for administrator approval.

![User Dashboard Screenshot](https://user-images.githubusercontent.com/1566301/66960196-38df6280-f029-11e9-8154-259031bbaa7a.png).

Their submissions will be summarized on the same dashboard as their Tripal Content for a unified experience!

### Administration
- Data file import forms are created automatically based on the TripalImporter::form() and TripalImporter:validate() is run on submission to ensure meta data matches standards. Administrators of this module do not need to create forms!
- Tripal HQ administration dashboard support approve/reject for Tripal Importer submissions.
<img width="1352" alt="Screen Shot 2019-10-16 at 3 25 18 PM" src="https://user-images.githubusercontent.com/1566301/66960197-38df6280-f029-11e9-840c-c97ea1a0f293.png">

## Future Development

- Support emails in the same manner as Tripal HQ
- Rich permissions for controlling which users have access to specific importers
- Rich permissions for deputizing users to approve/reject data imports as you can with Tripal HQ.
