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
 * @param $params - assumes the following keys exist:
 *      - user_name
 *
 */
function tripal_hq_mail($key, &$message, $params) {
  $language = $message['language'];
  $site_name = variable_get('site_name');
  $site_email = variable_get('site_mail');
  $user = $params['user_name'];

  switch ($key) {

    /**
     * Acknowledge user has submitted a content request.
     */
    case 'user_submit_notice':
      $site_settings = variable_get('tripal_submit_request');
      if ($site_settings['User'] == 0) {
        return;
      }
      $message['body'][] = t("Dear !user,", ['!user' => $user]);
      $message['subject'] = t('!site Content Submission Request Received', ['!site' => $site_name], ['langcode' => $language->language]);

      break;

    /**
     * Let relevant admins know a content request has been received.
     */
    case 'admin_submit_notice':
      $site_settings = variable_get('tripal_submit_request');
      if ($site_settings['Admin'] == 0) {
        return;
      }
      //TODO: flesh this out.
      $message['subject'] = t('!site Content Submission Request Received', ['!site' => $site_name], ['langcode' => $language->language]);
      $message['body'][] = t("Dear !user,", ['!user' => $user]);
      $message['body'][] = t("A user has submitted the following content creation request:");
      $message['body'][] = t("To approve or deny this request, please visit your Tripal HQ content portal, located here:");

      break;

    /**
     * User's submission has been approved!
     */
    case 'user_accept_notice':

      //TODO: flesh this out.


      break;

    /**
     * Admin: submission has been approved!
     */
    case 'admin_accept_notice':

      //TODO: flesh this out.


      break;
  }

  $message['body'][] = '<br><BR>Regards,<br><bR>The ' . $site_name . 'team';
  $message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
}