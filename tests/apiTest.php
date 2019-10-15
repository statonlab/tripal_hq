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

  /**
   * Tests tripal_hq_editview_form_field().
   */
  public function testEditViewFormField() {
    $form = [
      'element1' => [
        '#type' => 'textfield',
        '#disabled' => FALSE,
      ],
      'element2' => [
        '#type' => 'select',
        '#options' => [1,2,3,4],
      ],
      'containerblahblah' => [
        '#type' => 'fieldset',
        'element3' => [
          '#type' => 'textfield',
          '#disabled' => FALSE,
        ],
        'element4' => [
          '#type' => 'select',
          '#options' => [1,2,3,4],
        ],
        'container222' => [
          '#type' => 'fieldset',
          'element5' => [
            '#type' => 'select',
            '#options' => [1,2,3,4],
          ],
        ],
      ],
    ];
    $values = [
      'element1' => 'fred',
      'element2' => 3,
      'element3' => 'sarah',
      'element4' => 1,
      'element5' => 4,
    ];

    foreach (element_children($form) as $element_key) {
      tripal_hq_editview_form_field($form[$element_key], $values, $element_key, 'view');
    }

    // Check that everything is disabled.
    $this->assertTrue($form['element1']['#disabled'], 'element1 was not disabled.');
    $this->assertTrue($form['element2']['#disabled'], 'element2 was not disabled.');
    $this->assertTrue($form['containerblahblah']['element3']['#disabled'], 'element3 was not disabled.');
    $this->assertTrue($form['containerblahblah']['element4']['#disabled'], 'element4 was not disabled.');
    $this->assertTrue($form['containerblahblah']['container222']['element5']['#disabled'], 'element5 was not disabled.');

    // Check that their default value is set correctly.
    $this->assertEquals('fred', $form['element1']['#default_value'],
      'element1 default value was not set properly.');
    $this->assertEquals(3, $form['element2']['#default_value'],
      'element2 default value was not set properly.');
    $this->assertEquals('sarah', $form['containerblahblah']['element3']['#default_value'],
      'element3 default value was not set properly.');
    $this->assertEquals(1, $form['containerblahblah']['element4']['#default_value'],
      'element4 default value was not set properly.');
    $this->assertEquals(4, $form['containerblahblah']['container222']['element5']['#default_value'], 
      'element5 default value was not set properly.');
  }
}
