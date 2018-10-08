[![Build Status](https://travis-ci.org/statonlab/tripal_hq.svg?branch=master)](https://travis-ci.org/statonlab/tripal_hq)

This module is under active development and is not suitable for usage.

## Contributing

We're excited to work with you!  Post in the issues queue.

### Development

This module uses [Tripal Test Suite](https://tripaltestsuite.readthedocs.io/en/latest/installation.html#joining-an-existing-project).  It provides a database seeder to make development a bit easier: run `vendor/bin/tripaltest db:seed` to run all seeders.  This will create fictional users on the site with pending and approved HQ submissions.  **Do not run seeds on a production site**.