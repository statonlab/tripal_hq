<?php
namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class apiTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
  // use DBTransaction;

  /**
   * Tests tripal_hg_get_importers().
   */
  public function testGetImporters() {

    $importers = tripal_hq_get_importers();
    $this->assertIsArray($importers);
    $this->assertNotEmpty($importers);
  }

  /**
   * Tests tripal_hq_load_include_importer_class().
   */
  public function testLoadImporterClass() {

    // First test one which should work.
    $success = tripal_hq_load_include_importer_class('GFF3Importer');
    $this->assertTrue($success);

    // Next test one which fails.
    $success = tripal_hq_load_include_importer_class(uniqid());
    $this->assertFalse($success);
  }
}
