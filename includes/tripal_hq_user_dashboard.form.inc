<?php

function tripal_hq_user_dashboard_form() {
global $user;

dpm($user->uid);

$submissions = tripal_hq_get_user_submissions($user);

foreach ($submissions as $submission){



}

$form['my_submissions']= [];
  return $form;
}

function tripal_hq_get_user_submissions($user){
  $uid = $user->uid;

}
