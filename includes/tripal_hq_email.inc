<?php

/**
 * Implements hook_mail().
 *
 * @param $key
 *    One of the following strings:
 *      - user_submit_notice
 *      - admin_submit_notice
 *      - user_accept_notice
 *      - admin_accept_notice
 *      - user_reject_notice
 *      - admin_reject_notice
 *      - user_edit_notice
 *      - admin_edit_notice
 *
 * @param $message
 * @param $params - this is the tripal_hq_submission object.
 *
 */

function tripal_hq_mail($key, &$message, $params) {
  $language = $message['language'];
  $site_name = variable_get('site_name');
  $site_email = variable_get('site_mail');
  global $base_url;

  $uid = $params->uid;
  $user_object = user_load($uid);
  $submitter_name = $user_object->name;

  $title = $params->title;
  $bundle_id = $params->bundle_id;

  $bundle = tripal_load_bundle_entity(array('id' => $bundle_id));
  $bundle_label = $bundle->label;

  $entity_id = $params->entity_id;
  $link = $base_url . 'bio_data/'. $entity_id;



  switch ($key) {

    /**
     * Acknowledge user has submitted a content request.
     */
    case 'user_submit_notice':
      $site_settings = variable_get('tripal_hq_submit_request');
      if ($site_settings['User'] == 0) {
        return;
      }
      $message['body'][] = t("Dear !user,", ['!user' => $submitter_name]);
      $message['subject'] = t('!site Content Submission Request Received', ['!site' => $site_name], ['langcode' => $language->language]);
      $message['body'][] = t('You have successfully submitted a request to create the !type record !title .', ['!type' => $bundle_label, '!type' => $title]);
      $message['body'][] = t('Our site admins will review your request.');
      break;

    /**
     * Let relevant admins know a content request has been received.
     */
    case 'admin_submit_notice':
      $site_settings = variable_get('tripal_hq_submit_request');
      if ($site_settings['Admin'] == 0) {
        return;
      }
      //TODO: flesh this out.
      $message['subject'] = t('!site Content Submission Request Received', ['!site' => $site_name], ['langcode' => $language->language]);
      $message['body'][] = t("Dear !user,", ['!user' => $submitter_name]);
      $message['body'][] = t("A user has submitted the following content creation request:");
      $message['body'][] = t("To approve or deny this request, please visit your Tripal HQ content portal, located here:");

      break;

    /**
     * User's submission has been approved!
     */
    case 'user_accept_notice':

      $site_settings = variable_get('tripal_hq_approval');
      if ($site_settings['User'] == 0) {
        return;
      }

      $message['subject'] = t('!site Content Submission Request Approved', ['!site' => $site_name], ['langcode' => $language->language]);
      $message['body'][] = t("Dear !user,", ['!user' => $submitter_name]);


      break;

    /**
     * Admin: submission has been approved!
     */
    case 'admin_accept_notice':
      $site_settings = variable_get('tripal_hq_approval');
      if ($site_settings['Admin'] == 0) {
        return;
      }

      break;

    case 'user_reject_notice':
      $site_settings = variable_get('tripal_hq_denied');
      if ($site_settings['User'] == 0) {
        return;
      }
      $message['subject'] = t('!site Content Submission Request Denied', ['!site' => $site_name], ['langcode' => $language->language]);
      $message['body'][] = t("Dear !user,", ['!user' => $submitter_name]);

      break;

    case 'admin_reject_notice':
      $site_settings = variable_get('tripal_hq_denied');
      if ($site_settings['Admin'] == 0) {
        return;
      }
      $message['subject'] = t('!site Content Submission Request Denied', ['!site' => $site_name], ['langcode' => $language->language]);
      $message['body'][] = t("Dear !user,", ['!user' => $submitter_name]);

      break;
  }

  $message['body'][] = '<br/>Regards,<br/>The ' . $site_name . ' team';
  $message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
}