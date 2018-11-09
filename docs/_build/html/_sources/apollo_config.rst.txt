.. _ApolloConfig:

=====================
Apollo Configuration
=====================



The Tripal Apollo module makes several assumptions about your Apollo instances in order to connect.  If these assumptions dont hold true for your configuration, please let us know on the issue board at https://github.com/NAL-i5K/tripal_apollo/issues and we'll try to help.


Apollo 2
=========


Expected Group Names
---------------------

Tripal Apollo does not configure your user groups for you.  Tripal Apollo assumes that for each organism, with a particular genus and species, you have three groups: ``genus_species_ADMIN``, ``genus_species_USER``, and ``genus_species_WRITE``.  This is typically done when adding the organism to Apollo.  For this module to function to correctly, the organism you create on your Tripal site much have genus and species fields which match the existing groups on your Apollo instance.

The organism *Saccharocmyes cerevisiae* should therefore have groups configured ``saccharomyces_cerevisiae_WRITE``, ``saccharomyces_cerevisiae_USER``, and ``saccharomyces_cerevisiae_ADMIN``.  Approving a user request for this organism will add them to the first two.

Apollo 1
==========

Server Setup
--------------

Apollo 1 does not support a REST API.  Your Apollo 1 server's database must therefore be setup to accept remote connections by editing ``pg_hba.conf``.


Naming Conventions
~~~~~~~~~~~~~~~~~~~~

Tripal Apollo provides limited support for Apollo 1, via a python script which connects directly to the instance database.  Note this is why the **Database Name** field is only required for an Apollo 1 instance.
A key discrepancy between Apollo 1 and 2 is that each organism is its own server for Apollo 1.  Rather than require admins to create Apollo instances for each organism separately, we assume a uniform URL for Apollo 1 organisms attached to the same instance:

``http://url/[first three letters of genus][first three letters of species].``

If your URL is set as http://localhost:8000, and you select organisms *Acer saccharum* & *Homo sapiens*, for example, this Apollo 1 instance will connect to the following two apollo servers:

* http://localhost:8000/acesac
* http://localhost:8000/homsap
