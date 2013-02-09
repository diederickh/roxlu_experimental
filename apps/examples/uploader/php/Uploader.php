<?php

/**
 * Version 0.0.1
 * -------------
 * 
 * Uploader handler for uploader util. 
 * ----------------------------------
 * Example class which handles a file upload from the 
 * uploader utility. This class doesn't do a lot, except
 * handling a file upload correctly. It will log verbose
 * information if the 'log' flag is set in the configuration
 * array.
 *
 * Configuration:
 * --------------
 * - destination_path: the full path to the upload directory where you want 
 *                     to store the uploaded files.
 * 
 * - file_field:       the name in the $_FILES array that contains the 
 *                     name of the uploaded file.
 * 
 * - log:              set to true if you want to log
 * - log_file:         append log entries to this file
 * 
 * 
 *
 * Example usage:
 * --------------
 *
 *      $path = pathinfo(__FILE__);
 *      $base_dir = $path['dirname'] .'/';
 *      
 *      $config = array(
 *        'log' => true,
 *        'log_file' => $base_dir .'upload.log',
 *        'destination_path' => $base_dir .'uploaded/',
 *        'file_field' => 'file'
 *      );
 *      
 *      require_once $base_dir .'Uploader.php';
 *      $up = new Uploader($config);
 *      
 *      if($up->execute() !== false) {
 *        echo "yay uploaded!";
 *      }
 *            
 */

class Uploader {
  private $config = NULL;

  function __construct($config) {
    $this->config = $config;
    if(!$this->init()) {
      die('Could not initialize the uploader. Check the logs');
    }
  }

  private function init() {
    if(array_key_exists('log', $this->config)
      && $this->config['log'])
      {
        if(!array_key_exists('log_file', $this->config)) {
          die('ERROR: you asked me to log, but the log_file configuration is not set.');
        }
        else if(!is_writable($this->config['log_file'])) {
          die('ERROR: the log file is not writable (e.g. $ touch logfile.log && chmod 777 logfile.log)');
        }
      }

    if(!array_key_exists('file_field', $this->config)) {
      $this->log('ERROR: the file_field config is not set. ', true);
      return false;
    }

    if(!is_writable($this->config['destination_path'])) {
      $this->log('ERROR: the upload path is not writable or does not exist', true);
      return false;
    }
    
    return true;
  }

  /**
   * Handles the file upload. For now we only handle one
   * file at a time; we do iterate over all the files though, we might
   * return an array of uploaded files in the next versionb but for now 
   * we return just the path of the found file entry.
   *
   * @return mixed false  - We return false on error
   * @return mixed string - The path to the uploaded file on success
   */
  public function execute() {
    $this->log('new request');
    if(!is_array($_FILES) || count($_FILES) == 0) {
      return;
    }

    $uploaded_file = '';
    $ok = true;
    foreach($_FILES as $upname => $file) {
      if($upname != $this->config['file_field']) {
        $this->log('ERROR: we received a file but the name of the file is not the same as in the config.');
        continue;
      }
      
      $dest_file = $this->config['destination_path'] .$file['name'];
      $uploaded_file = $dest_file;
      $result = move_uploaded_file($file['tmp_name'], $dest_file);
      
      if(!$result) {
        $ok = false;
        $str = 'ERROR: ';
        switch($file['error']) {
          case UPLOAD_ERR_INI_SIZE:    $str .= 'The uploaded file exceeds the upload_max_filesize directive in php.ini.'; break;
          case UPLOAD_ERR_FORM_SIZE:   $str .= 'The uploaded file exceeds the MAX_FILE_SIZE directive that was specified in the HTML form'; break;
          case UPLOAD_ERR_PARTIAL:     $str .= 'The uploaded file was only partially uploaded.'; break;
          case UPLOAD_ERR_NO_FILE:     $str .= 'No file was uploaded.'; break;
          case UPLOAD_ERR_NO_TMP_DIR:  $str .= 'Missing a temporary folder. Introduced in PHP 4.3.10 and PHP 5.0.3.'; break;
          case UPLOAD_ERR_CANT_WRITE:  $str .= 'Failed to write file to disk. Introduced in PHP 5.1.0'; break; 
          case UPLOAD_ERR_EXTENSION:   $str .= 'A PHP extension stopped the file upload'; break;
          default: $str .= ' UNKNOWN ERROR '; break;
        };
        $this->log($str);
      }
    }
    if(!$ok) {
      return $ok;
    }
    return $uploaded_file;
  }


  private function log($s, $die = false) {
    if(!$this->config['log']) {
      if($die) {
        die($s);
      }
      return;
    }
    $msg = date('Y-m-d H:i ', time()) .$s ."\n";
    file_put_contents($this->config['log_file'], $msg);
    if($die) {
      die($s);
    }
  }
  
}