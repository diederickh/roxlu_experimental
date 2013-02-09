<?php
function e($s) { echo $s; echo '<br>'; }
function p($a) { echo '<pre>'; print_r($a); echo '</pre>'; }

$path = pathinfo(__FILE__);
$base_dir = $path['dirname'] .'/';

$config = array(
  'log' => true,
  'log_file' => $base_dir .'upload.log',
  'destination_path' => $base_dir .'uploaded/',
  'file_field' => 'file'
);

require_once $base_dir .'Uploader.php';
$up = new Uploader($config);

if($up->execute() !== false) {
  // do something after success
}

