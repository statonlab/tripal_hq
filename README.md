[![Build Status](https://travis-ci.org/statonlab/tripal_hq.svg?branch=master)](https://travis-ci.org/statonlab/tripal_hq)

#Tripal HeadQuarters (HQ)

Tripal HQ provides an administative curation toolbox for your Tripal site.  This means that users are able to create whatever Chado content you'd like them, but withhold inserting it into the database until someone has approved it.


This module is under active development and is not suitable for usage.

## Module Features

* Users create data using your existing Bundle configuration- no extra forms! (Coming Soon)
* User dashboard area for viewing pending submissions (Coming soon)
* Admin dashboard for viewing submissions (Coming soon)
* Chado-based permissions to create admins for certain projects or organisms (Not implemented/additional extension module).

## Contributing

We're excited to work with you!  Post in the issues queue.

### Development

This module uses [Tripal Test Suite](https://tripaltestsuite.readthedocs.io/en/latest/installation.html#joining-an-existing-project).  It provides a database seeder to make development a bit easier: run `vendor/bin/tripaltest db:seed` to run all seeders.  This will create fictional users on the site with pending and approved HQ submissions.  **Do not run seeds on a production site**.