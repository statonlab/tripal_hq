======================================
Testing, Development, and Contributing
======================================

Testing
========

Tripal Apollo uses Tripal Test Suite to make configuring PHPUnit to work with Drupal and Tripal easy.  Tripal Test Suite documentation is available here: https://tripaltestsuite.readthedocs.io/en/latest/


Creating a Development Environment
-----------------------------------

The travis CI environment uses the Docker compose file in this repository to launch a Tripal site and Apollo site.  You can simply use this configuration locally!


 An example setup:

.. code-block:: bash

  # extract the example dataset
  tar -xvf example_data/yeast.tar.gz -C example_data/
  composer install
  docker-compose up -d
  ## Set the APOLLO_URL variable.
  APOLLO_URL=http://localhost:8888
  export APOLLO_URL
  /bin/bash setup/set_travis_apollo.sh

If you only need an Apollo container, it can be run via ``docker run``:

.. code-block:: bash

  # extract the example dataset
  tar -xvf example_data/yeast.tar.gz -C example_data/
  # run an Apollo container
  docker run -it -v ${PWD}/example_data/:/data  -p 8888:8080 quay.io/gmod/docker-apollo:2.1.0

  ## Set the APOLLO_URL variable.
  APOLLO_URL=http://localhost:8888
  export APOLLO_URL
  #run the setup script, which will create the organism and groups in the Apollo instance.
  /bin/bash setup/set_travis_apollo.sh


.. note::

  The Apollo credentials for this container are:

  * username: admin@local.host
  * password: password

Setting up Tripal Test Suite
-----------------------------

Prior to running test suite, you must run ``composer install`` and copy ``tests/example.env`` to ``tests/.env``.  Note we define an extra variable in ``tests/example.env``: ``APOLLO_URL=http://localhost:8888``.  This **MUST** include ``http://`` and it must point at your Apollo instance for tests to work.

See https://tripaltestsuite.readthedocs.io/en/latest/environment.html?highlight=.env for general information on setting up Test Suite.


Contributing
=============

Tripal Apollo is open source and distributed via the GPL 3 license.  If you have questions, feature requests, or a desire to contribute, please post to the github issues board here: https://github.com/NAL-i5K/tripal_apollo/issues
