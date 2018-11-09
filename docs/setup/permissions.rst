===========================
Configuring HQ Permissions
===========================

Permissions and Roles
=======================

On installation, HQ defines the following permissions:

* Administer HQ content:  For Admins to approve/deny content.  Can be combined with the next permission to allow administration of only a subset of requests.
* Administer CHADO-specific tripal_HQ content: Lets you specify what Chado content a user can administer.
* Create Tripal content requests: Allows users to submit content requests and view their dashboard.
* "Propose content" permission for each of your defined bundles.  This will let you configure which bundles can be proposed by users.


To get started, you'll need to create a role for your content submitters, and give them the "Create Content Requests" permission, plus whatever specific bundle permissions you'd like them to see.

For your administrators, you'll want to give them the "Administer HQ content" permission.  If you'd like your admin to only see a subset of content, give them the "Administer CHADO-specific tripal_HQ content" permission and configure their specific permissions (see below).


.. note::

  You must create a role for these permissions.  Using the default "Authenticated User" role will **not work**.


Chado specific Permissions
--------------------------

Chado-specific permissions can be configured at ```admin/tripal/tripal_hq/chado_permissions``.  Here you will see all users with the "Administer CHADO-specific tripal_HQ content" permission.  Click "Assign" to give them permission for specific organisms.

.. image:: img/permissions_page.png

Select one or more organism for this user and click "Submit".

.. image:: img/specific_permission.png


For now, you can only configure organism-based permissions.
