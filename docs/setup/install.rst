=============
Installation
=============


Installing Tripal HQ
======================

Clone the repo with github and enable the module with drush.  For example:


.. code-block:: bash

  cd /var/www/html/sites/all/modules/custom/
  git clone https://github.com/statonlab/tripal_hq.git
  drush pm-enable tripal_hq tripal_hq_permissions



Additional Module Installation
================================


.. _install_field_permissions:

Field Permissions
------------------

We recommend using the Drupal Field Permissions module to hide some fields from user submission forms.  You can read more about why this is a good idea :ref:`why_field_permissions`

The module can be enabled directly from Drush with the below command.

.. code-block:: bash

  drush pm-enable -y field_permissions


You can find the Field Permission module page here: https://www.drupal.org/project/field_permissions and a more in-depth user guide here: https://www.drupal.org/node/2802067
