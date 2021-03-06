<?php

/**
 * @file
 * Editor integration functions for ueditor.
 */

/**
 * Plugin implementation of hook_editor().
 */
function ueditor_ueditor_editor() {
  $editor['ueditor'] = array(
    'title' => 'Ueditor',
    'vendor url' => 'http://ueditor.baidu.com/website/index.html',
    'download url' => 'http://ueditor.baidu.com/website/download.html',
    'libraries' => array(
      '' => array(
        'title' => 'Source',
        'files' => array('ueditor.all.js'),
      ),
    ),
    'version callback' => 'ueditor_version',
    'settings callback' => 'ueditor_settings',
    'settings form callback' => 'ueditor_settings_form',
    'versions' => array(
      '1.4.3' => array(
        'js files' => array('ueditor.js'),
      ),
    ),
  );
  return $editor;
}

/**
 * Detect editor version.
 *
 * @param array $editor
 *   An array containing editor properties as returned from hook_editor().
 *
 * @return int
 *   The installed editor version.
 */
function ueditor_version($editor) {
  $script = $editor['library path'] . '/ueditor.all.js';
  if (!file_exists($script)) {
    return;
  }
  $script = fopen($script, 'r');
	$max_lines = 50;
  while ($max_lines && $line = fgets($script, 500)) {    
    if (preg_match('/^UE.*?"([\d\.]+)"/', $line, $version)) {
      fclose($script);
      return $version[1];
    }
    $max_lines--;
  }
	fclose($script);
}

/**
 * Enhances the editor profile settings form for UEditor.
 */
function ueditor_settings_form(&$form, &$form_state) {
	if(!empty($form_state['wysiwyg_profile']->settings['toolbars'])){
		$settings = $form_state['wysiwyg_profile']->settings;
	}else{
		$settings = ueditor_config_default($form_state['wysiwyg_profile']->settings);
		$settings['toolbars'] = _ueditor_toolbars($settings['toolbars']);
	}

	$form['basic']['zindex'] = array(
    '#type' => 'textfield',
    '#title' => t('Editor zindex'),
    '#description' => t('The official website of the default zindex 900,<br />
      and Drupal overlay module ( #overlay= page ) conflict,
      so default change from 900 to 90 or you can customize.'),
    '#default_value' => $settings['zindex'],
    '#size' => 5,
    '#maxlength' => 4,
    '#required' => TRUE,
  );
  $form['basic']['initial_content'] = array(
    '#type' => 'textfield',
    '#title' => t('Editor initial content'),
    '#description' => t('Editor initial content, after editor loading in the textarea.'),
    '#default_value' => $settings['initial_content'],
    '#maxlength' => 255,
  );
  $form['basic']['allowdivtop'] = array(
    '#type' => 'checkbox',
		'#title' => t('Allow Div Convert to P'),
		'#default_value' => $settings['allowdivtop'],
		'#description' => t('If enable, the Div tags converted to P tag.'),
  );
  $form['basic']['auto_height'] = array(
    '#type' => 'checkbox',
		'#title' => t('Auto Height'),
		'#default_value' => $settings['auto_height'],
		'#description' => t('If enable, the editor will auto height.'),
  );
  $form['basic']['auto_float'] = array(
    '#type' => 'checkbox',
		'#title' => t('Auto Float'),
		'#default_value' => $settings['auto_float'],
		'#description' => t('If enable, the editor will auto float.'),
  );
	$form['basic']['appearance'] = array(
    '#type'         => 'fieldset',
    '#title'        => t('Appearance'),
    '#collapsible'  => TRUE,
    '#collapsed'    => FALSE,		
		'#attributes'			=> array('class' => array('edui-default')),
  );
  $form['basic']['appearance']['toolbars'] = array(
    '#type'           => 'textarea',
    '#title'          => t('Toolbars'),
    '#default_value'  => isset($settings['toolbars']) ? $settings['toolbars'] : '',
    '#description'    => t('Enter a comma separated list of toolbars.'),
  );

	drupal_add_js('Drupal.ueditor.admin_profile_form();', array('scope' => 'footer', 'type' => 'inline'));

	// config the upload path.
	$form['basic']['uploadpath'] = array(
    '#type'         => 'fieldset',
    '#title'        => t('Custom Upload Path'),
    '#collapsible'  => TRUE,
    '#collapsed'    => TRUE,
  );
	$form['basic']['uploadpath']['path_help'] = array(
    '#markup' => implode('<br>', array(
      '%b' => '<code>%b</code> - the base URL path of the Drupal installation (<code>'._ueditor_realpath('%b').'</code>)',
      '%m' => '<code>%m</code> - path where the UEditor module is stored (<code>'._ueditor_realpath('%m').'</code>)',
      '%l' => '<code>%l</code> - path to the libraries directory (<code>'._ueditor_realpath('%l').'</code>)',
      '%f' => '<code>%f</code> - the Drupal file system path where the files are stored (<code>'._ueditor_realpath('%f').'</code>)',
      '%d' => '<code>%d</code> - the server path to the document root (<code>'._ueditor_realpath('%d').'</code>)',
      '%u' => '<code>%u</code> - User ID (<code>'._ueditor_realpath('%u').'</code>)',
			'{yyyy}' => '<code>{yyyy}</code> - <a href="http://www.php.net/manual/en/function.date.php">The php date format</a>',
			'{mm}' => '<code>{mm}</code> - <a href="http://www.php.net/manual/en/function.date.php">The php date format</a>',
			'{dd}' => '<code>{dd}</code> - <a href="http://www.php.net/manual/en/function.date.php">The php date format</a>',
			'{time}' => '<code>{time}</code> - A timestamp',
			'{rand:6}' => '<code>{rand:6}</code> - A random number',
      '<br>',
    )),
    '#prefix' => '<div class="region region-help"><div class="block block-system"><div class="content">',
    '#suffix' => '</div></div></div>',
  );

	$imagePathFormat = !empty($settings['imagePathFormat']) ? $settings['imagePathFormat'] : '';
  $form['basic']['uploadpath']['imagePathFormat'] = array(
    '#type'           => 'textfield',
    '#title'          => t('imagePathFormat'),
    '#default_value'  => $imagePathFormat,
    '#description'    => 'Current path:<code>'._ueditor_realpath($imagePathFormat).'</code>',
  );
  $scrawlPathFormat = !empty($settings['scrawlPathFormat']) ? $settings['scrawlPathFormat'] : '';
  $form['basic']['uploadpath']['scrawlPathFormat'] = array(
    '#type'           => 'textfield',
    '#title'          => t('scrawlPathFormat'),
    '#default_value'  => $scrawlPathFormat,
    '#description'    => 'Current path:<code>'._ueditor_realpath($scrawlPathFormat).'</code>',
  );
  $filePathFormat = !empty($settings['filePathFormat']) ? $settings['filePathFormat'] : '';
  $form['basic']['uploadpath']['filePathFormat'] = array(
    '#type'           => 'textfield',
    '#title'          => t('filePathFormat'),
    '#default_value'  => $filePathFormat,
    '#description'    => 'Current path:<code>'._ueditor_realpath($filePathFormat).'</code>',
  );
  $fileManagerListPath = !empty($settings['fileManagerListPath']) ? $settings['fileManagerListPath'] : '';
  $form['basic']['uploadpath']['fileManagerListPath'] = array(
    '#type'           => 'textfield',
    '#title'          => t('fileManagerListPath'),
    '#default_value'  => $fileManagerListPath,
    '#description'    => 'Current path:<code>'._ueditor_realpath($fileManagerListPath).'</code>',
  );
  $catcherPathFormat = !empty($settings['catcherPathFormat']) ? $settings['catcherPathFormat'] : '';
  $form['basic']['uploadpath']['catcherPathFormat'] = array(
    '#type'           => 'textfield',
    '#title'          => t('catcherPathFormat'),
    '#default_value'  => $catcherPathFormat,
    '#description'    => 'Current path:<code>'._ueditor_realpath($catcherPathFormat).'</code>',
  );
  $imageManagerListPath = !empty($settings['imageManagerListPath']) ? $settings['imageManagerListPath'] : '';
  $form['basic']['uploadpath']['imageManagerListPath'] = array(
    '#type'           => 'textfield',
    '#title'          => t('imageManagerListPath'),
    '#default_value'  => $imageManagerListPath,
    '#description'    => 'Current path:<code>'._ueditor_realpath($imageManagerListPath).'</code>',
  );
  $snapscreenPathFormat = !empty($settings['snapscreenPathFormat']) ? $settings['snapscreenPathFormat'] : '';
  $form['basic']['uploadpath']['snapscreenPathFormat'] = array(
    '#type'           => 'textfield',
    '#title'          => t('snapscreenPathFormat'),
    '#default_value'  => $snapscreenPathFormat,
    '#description'    => 'Current path:<code>'._ueditor_realpath($snapscreenPathFormat).'</code>',
  );
  $videoPathFormat = !empty($settings['videoPathFormat']) ? $settings['videoPathFormat'] : '';
  $form['basic']['uploadpath']['videoPathFormat'] = array(
    '#type'           => 'textfield',
    '#title'          => t('videoPathFormat'),
    '#default_value'  => $videoPathFormat,
    '#description'    => 'Current path:<code>'._ueditor_realpath($videoPathFormat).'</code>',
  );
	$form['basic']['uploadpath']['#element_validate'][] = 'ueditor_settings_form_validate_uploadpath';
}

/**
 * Deal with settings form submit.
 */
function ueditor_settings_form_validate_uploadpath($element, &$form_state) {
	$uploadpath = array();
	if(!empty($element['imagePathFormat']['#value'])){
		$uploadpath['imagePathFormat'] = $element['imagePathFormat']['#value'];
	}
	if(!empty($element['scrawlPathFormat']['#value'])){
		$uploadpath['scrawlPathFormat'] = $element['scrawlPathFormat']['#value'];
	}
	if(!empty($element['filePathFormat']['#value'])){
		$uploadpath['filePathFormat'] = $element['filePathFormat']['#value'];
	}
	if(!empty($element['fileManagerListPath']['#value'])){
		$uploadpath['fileManagerListPath'] = $element['fileManagerListPath']['#value'];
	}
	if(!empty($element['catcherPathFormat']['#value'])){
		$uploadpath['catcherPathFormat'] = $element['catcherPathFormat']['#value'];
	}
	if(!empty($element['imageManagerListPath']['#value'])){
		$uploadpath['imageManagerListPath'] = $element['imageManagerListPath']['#value'];
	}
	if(!empty($element['snapscreenPathFormat']['#value'])){
		$uploadpath['snapscreenPathFormat'] = $element['snapscreenPathFormat']['#value'];
	}
	if(!empty($element['videoPathFormat']['#value'])){
		$uploadpath['videoPathFormat'] = $element['videoPathFormat']['#value'];
	}

	if(!empty($uploadpath)){
		variable_set('ueditor_uploadpath_config', $uploadpath);
	}
}

/**
 * Return runtime editor settings for a given wysiwyg profile.
 *
 * @param array $editor
 *   A processed hook_editor() array of editor properties.
 * @param array $config
 *   An array containing wysiwyg editor profile settings.
 * @param string $theme
 *   The name of a theme/GUI/skin to use.
 *
 * @return array
 *   A settings array to be populated in
 *   Drupal.settings.wysiwyg.configs.{editor}
 */
function ueditor_settings($editor, $config, $theme) {
  // Settings.
	$settings = $config;
  $settings['editorPath'] = base_path() . $editor['library path'] . '/';

	// Change the chinese language code.
	if(isset($settings['language']) && $settings['language'] == 'zh-hans'){
		$settings['language'] = 'zh-cn';
	}
	
  $settings['toolbars'] = (!empty($config['toolbars']) ? array(explode(',',$config['toolbars'])) : '');
	$settings['serverUrl'] = base_path().'ueditor/controller';

	// Load config js.
  drupal_add_js($editor['library path'] . '/ueditor.config.js', array('weight' => -1));

  // Theme.
  // Ueditor temporarily does not support multiple themes,
  // Can customize CSS overwrite.
  drupal_add_css($editor['library path'] . '/themes/default/css/ueditor.css');
  drupal_add_css($editor['library path'] . '/themes/default/iframe.css');

  return $settings;
}

/**
 * Convert toolbars array of string.
 */
function _ueditor_toolbars($toolbars) {
  if(!empty($toolbars[0])) {
    $t = $toolbars[0];
    return implode(',', $t);
  }
  return '';
}

/**
 * Default ueditor config array.
 */
function ueditor_config_default(&$settings) {
	$settings['language'] = 'en';
	$settings['initial_content'] = '';
	$settings['zindex'] = 500;
	$settings['auto_height'] = TRUE;
	$settings['auto_float'] = TRUE;
	$settings['allowdivtop'] = FALSE;
	$settings['imagePathFormat'] = '/%b%f/%u/ueditor/upload/image/{yyyy}{mm}{dd}/{time}{rand:6}';
	$settings['scrawlPathFormat'] = '/%b%f/%u/ueditor/upload/scrawl/{yyyy}{mm}{dd}/{time}{rand:6}';
	$settings['filePathFormat'] = '/%b%f/%u/ueditor/upload/file/{yyyy}{mm}{dd}/{time}{rand:6}';
	$settings['fileManagerListPath'] = '/%b%f/%u/ueditor/upload/file/';
	$settings['catcherPathFormat'] = '/%b%f/%u/ueditor/upload/catcher/{yyyy}{mm}{dd}/{time}{rand:6}';
	$settings['imageManagerListPath'] = '/%b%f/%u/ueditor/upload/image/';
	$settings['snapscreenPathFormat'] = '/%b%f/%u/ueditor/upload/snapscreen/{yyyy}{mm}{dd}/{time}{rand:6}';
	$settings['videoPathFormat'] = '/%b%f/%u/ueditor/upload/video/{yyyy}{mm}{dd}/{time}{rand:6}';
	$settings['toolbars'] = array(
														array('fullscreen', 'source', '|', 'undo', 'redo', '|',
																	'bold', 'italic', 'underline', 'fontborder', 'strikethrough', 'superscript', 'subscript', 'removeformat', 'formatmatch', 'autotypeset', 'blockquote', 'pasteplain', '|', 'forecolor', 'backcolor', 'insertorderedlist', 'insertunorderedlist', 'selectall', 'cleardoc', '|',
																	'rowspacingtop', 'rowspacingbottom', 'lineheight', '|',
																	'customstyle', 'paragraph', 'fontfamily', 'fontsize', '|',
																	'directionalityltr', 'directionalityrtl', 'indent', '|',
																	'justifyleft', 'justifycenter', 'justifyright', 'justifyjustify', '|', 'touppercase', 'tolowercase', '|',
																	'link', 'unlink', 'anchor', '|', 'imagenone', 'imageleft', 'imageright', 'imagecenter', '|',
																	'simpleupload', 'insertimage', 'emotion', 'scrawl', 'insertvideo', 'music', 'attachment', 'map', 'gmap', 'insertframe', 'insertcode', 'webapp', 'pagebreak', 'template', 'background', '|',
																	'horizontal', 'date', 'time', 'spechars', 'snapscreen', 'wordimage', '|',
																	'inserttable', 'deletetable', 'insertparagraphbeforetable', 'insertrow', 'deleterow', 'insertcol', 'deletecol', 'mergecells', 'mergeright', 'mergedown', 'splittocells', 'splittorows', 'splittocols', 'charts', '|',
																	'print', 'preview', 'searchreplace', 'help', 'drafts'),
												 );

	return $settings;
}
