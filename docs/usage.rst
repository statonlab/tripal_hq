=============
Module Usage
=============

Users
=======

Content Pages
---------------

User content submission can be reached at ``tripal_hq/bio_data``.


.. image:: /_static/img/user_dash.png

Approved, pending, and rejected content is displayed in the table.  Clicking the View/Edit button will either link to the accepted entity, or to the submission form to submit changes.

Click the "Submit Content" button to submit Tripal Content.  The submission form is the **exact same form** as the admin content creation form.

Data Files
------------

Data files can be submitted by users at ``tripal_hq/bio_data/import-data``.

.. image:: /_static/img/imports_userdash.png

As you can see in the above screenshot when Tripal HQ Imports is enabled, it adds a second table to the user dashboard. This makes it easy to see the status of both your content and data file submissions.

To submit a new data file with associated metadata, click "Import data file". You can edit a pending submission by clicking the "Edit" link beside that particular data file. Both the submission add and edit forms match the administrative data import forms exactly which ensures consistent metadata and validation.

Admins
=======

 The HQ admin dashboard is located at ``tripal_hq/admin``.  Admins can click on the tabs to filter submissions based on their status.


.. image:: /_static/img/admin_dash.png


Clicking on the **title** of a pending submission will bring up the edit form, and an admin can make changes to the submission before it is created.

If Chado-specific permissions are set, the admin will only see content on this page related to the organism they are Deputy of.
