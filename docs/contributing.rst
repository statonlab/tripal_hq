============
Contributing
============

We're excited to work with you!  Post in the issues queue with any questions, feature requests, or proposals.

Development
-----------

This module uses `Tripal Test Suite <https://tripaltestsuite.readthedocs.io/en/latest/installation.html#joining-an-existing-project>`_.
It provides a database seeder to make development a bit easier. Once you've installed test suite (``composer install``): run ``vendor/bin/tripaltest db:seed`` to run all seeders.  This will create fictional users on the site with pending and approved HQ submissions.

.. warning::

	**NEVER** run seeders on production sites. They will insert fictitious data into Chado.
