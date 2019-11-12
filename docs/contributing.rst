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

Coding Standards
------------------

This project uses code climate to ensure coding standards are met. We suggest you use php_codesniffer locally to check coding standards before submitting a Pull Request for a smoother experience. This can be done as follows:

1. Run ``composer up`` within the Tripal HQ directory. This will install php_codesniffer locally.
2. Check coding standards by running ``./vendor/bin/phpcs --standard=vendor/drupal/coder/coder_sniffer/Drupal/ruleset.xml [file]`` where ``[file]`` contains your changes. This will output a report meant to help you improve your code.
3. php_codesniffer includes a tool for automatically fixing many warnings you may have encountered. To run it execute ``./vendor/bin/phpcbf --standard=vendor/drupal/coder/coder_sniffer/Drupal/ruleset.xml [file]`` on the same file. Make sure to review any changes it makes.
4. Manually fix any remaining errors and re-run step 2 to confirm.

We truely appretiate your effort in keeping our project standards compliant!

Automated Testing
-------------------

This project uses TripalTestSuite and phpunit for automated testing. To run tests:

1. Run ``composer up`` within the Tripal HQ directory. This will install phpunit locally.
2. Run ``.vendor/bin/phpunit`` to execute all tests.
