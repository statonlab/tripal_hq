===========================
Field Specific Permissions
===========================


.. _why_field_permissions:

Why Field Permissions?
========================

Tripal HQ uses the existing Tripal content creation forms to build submission forms for your users.  One potential downside of doing this is some fields are confusing to end users.  This might be the case for a variety of reasons.  Some Chado base tables may have **type** fields that you don't utilize: for example, the contact table.  Some of your bundles may be configured with a lot of property fields, with only a subset of them being relevant to an end user submitting data via HQ.  Some fields are just not intuitive without some Chado experience: for example, the DBXREF field.

Simply disabling the display of the formatter won't prevent the widget from showing up on the submission page, and besides, you might want site admins to still have access to those fields!  Field Permissions allows you to configure field-specific permissions so that users contributing content via Chado only see the fields they need to see.

Please see :ref:`install_field_permissions` for instructions installing the field permissions module.

Setting Field-specific Permissions
===================================

Let's assume I want to hide the Cross-Reference field from my users submitting Genome Assembly data, but still want it available for my administrators.

.. image:: /_static/img/cross_ref_GA.png

First, navigate to the bundle field configuration page via **Admin --> Structure --> Tripal Content --> Genome Assembly**.  For each field we want to hide, we must configure the field instance settings individually.  Click **Edit** for the Cross Reference field, and scroll down to **CROSS REFERENCE FIELD SETTINGS**.
Select **Custom Permissions** and ensure that the user role you set up for HQ submitters can view, but cannot edit, this field.

.. image:: /_static/img/crossref_permissions.png

Once permissions are configured to your liking, click **Save Settings**.


.. warning::

  Some fields are **Required**.  Do not disable required fields that can't be null.  If you do, users won't be able to submit content!


Now, if you submit content via Tripal HQ as a user with that role, the field will not display on the widgets, but will still appear on normal content.
