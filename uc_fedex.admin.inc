<?php
// $Id$

/**
 * @file
 * FedEx Shipping Quotes module administration menu and callbacks
 *
 * @author Tim Rohaly.
 * @version $Id$
 */


/**
 * Default FedEx Web Services API settings.
 *
 * Records FedEx account information neccessary to use service. Allows testing
 * or production mode. Configures which FedEx services are quoted to customers.
 *
 * @return
 *   Forms for store administrator to set configuration options.
 */
function uc_fedex_admin_settings() {

  // Container for credentials forms
  $form['uc_fedex_credentials'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Credentials'),
    '#description'   => t('Account number and authorization information'),
    '#collapsible'   => TRUE,
    '#collapsed'     => FALSE,
  );

  // Form to set the developer key
  $form['uc_fedex_credentials']['uc_fedex_user_credential_key'] = array(
    '#type'          => 'textfield',
    '#title'         => t('FedEx Web Services API User Key'),
    '#default_value' => variable_get('uc_fedex_user_credential_key', ''),
    '#required'      => TRUE,
  );

  // Form to set the developer password
  $form['uc_fedex_credentials']['uc_fedex_user_credential_password'] = array(
    '#type'          => 'password',
    '#title'         => t('FedEx Web Services API Password'),
    '#default_value' => variable_get('uc_fedex_user_credential_password', ''),
  );

  // Form to set user account number
  $form['uc_fedex_credentials']['uc_fedex_account_number'] = array(
    '#type'          => 'textfield',
    '#title'         => t('FedEx Account #'),
    '#default_value' => variable_get('uc_fedex_account_number', 0),
    '#required'      => TRUE,
  );

  // Form to set user meter number
  $form['uc_fedex_credentials']['uc_fedex_meter_number'] = array(
    '#type'          => 'textfield',
    '#title'         => t('FedEx Meter #'),
    '#default_value' => variable_get('uc_fedex_meter_number', 0),
    '#required'      => TRUE,
  );

  // Form to set choose between Production and Testing server
  // ***Defaults to Testing!***
  $form['uc_fedex_credentials']['uc_fedex_server_role'] = array(
    '#type'          => 'select',
    '#title'         => t('FedEx Server Role'),
    '#description'   => t('Use the Testing server while developing and configuring your site.  Switch to the Production server only after you have demostrated that transactions on the Testing server are working and you are ready to go live.'),
    '#options'       => array(
      'testing'      => t('Testing'),
      'production'   => t('Production'),
    ),
    '#default_value' => variable_get('uc_fedex_server_role', 'testing'),
  );

  // Container for service selection forms
  $form['uc_fedex_service_selection'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Service Options'),
    '#collapsible'   => TRUE,
    '#collapsed'     => FALSE,
  );

  // Form to restrict FedEx services available to customer
  $form['uc_fedex_service_selection']['uc_fedex_services'] = array(
    '#type'          => 'checkboxes',
    '#title'         => t('FedEx Services'),
    '#default_value' => variable_get('uc_fedex_services', _uc_fedex_services()),
    '#options'       => _uc_fedex_services(),
    '#description'   => t('Select the FedEx services customers are allowed to use.'),
  );

  // Container for quote options forms
  $form['uc_fedex_quote_options'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Quote Options'),
    '#description'   => t('Preferences that affect computation of quote.'),
    '#collapsible'   => TRUE,
    '#collapsed'     => FALSE,
  );

  // Form to set choose between LIST quotes and ACCOUNT quotes
  // ***Defaults to LIST!***
  $form['uc_fedex_quote_options']['uc_fedex_quote_type'] = array(
    '#type'          => 'select',
    '#title'         => t('FedEx Quote Type'),
    '#description'   => t('Choose to present the customer with FedEx list prices or your discounted FedEx account prices. LIST prices only exist for US shipments - if you specify LIST for international shipments you will not receive any quotes.  Note that ACCOUNT prices are accurate only on the PRODUCTION server!'),
    '#options'       => array(
      'list'    => t('List Prices'),
      'account' => t('Discount Account Prices'),
    ),
    '#default_value' => variable_get('uc_fedex_quote_type', 'list'),
  );

  // Form to set how the package is handed over to FedEx
  $form['uc_fedex_quote_options']['uc_fedex_dropoff_type'] = array(
    '#type'          => 'select',
    '#title'         => t('FedEx Pickup/Dropoff Options'),
    '#default_value' => variable_get('uc_fedex_dropoff_type', _uc_fedex_dropoff_types()),
    '#options'       => _uc_fedex_dropoff_types(),
    '#description'   => t('Pickup/Dropoff options.  It is assumed that all your packages are using the same method.'),
  );

  // Form to select FedEx packaging to use
  $form['uc_fedex_quote_options']['uc_fedex_package_type'] = array(
    '#type'          => 'select',
    '#title'         => t('FedEx Package Type'),
    '#default_value' => variable_get('uc_fedex_package_type', _uc_fedex_package_types()),
    '#options'       => _uc_fedex_package_types(),
    '#description'   => t('Package Type.  It is assumed that all your packages are using the same packaging.'),
  );

  // Form to select packaging type
  $form['uc_fedex_quote_options']['uc_fedex_all_in_one'] = array(
    '#type'          => 'radios',
    '#title'         => t('Number of Packages'),
    '#default_value' => variable_get('uc_fedex_all_in_one', 1),
    '#options' => array(
      0 => t('Each product in its own package'),
      1 => t('All products in one package'),
    ),
    '#description'   => t('Indicate whether each product is quoted as shipping separately or all in one package.'),
  );

  // Form to add optional Insurance coverage in the amount of the order
  $form['uc_fedex_quote_options']['uc_fedex_insurance'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Add Insurance to shipping quote'),
    '#default_value' => variable_get('uc_fedex_insurance', FALSE),
    '#description'   => t('When enabled, products are insured for their full value.'),
  );

  // Container for markup forms
  $form['uc_fedex_markups'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Markups'),
    '#description'   => t('Modifiers to the shipping weight and quoted rate'),
    '#collapsible'   => TRUE,
    '#collapsed'     => FALSE,
  );

  // Form to select type of rate markup
  $form['uc_fedex_markups']['uc_fedex_rate_markup_type'] = array(
    '#type'          => 'select',
    '#title'         => t('Rate Markup Type'),
    '#default_value' => variable_get('uc_fedex_rate_markup_type', 'percentage'),
    '#options'       => array(
      'percentage' => t('Percentage (%)'),
      'multiplier' => t('Multiplier (×)'),
      'currency'   => t('Addition (!currency)', array('!currency' => variable_get('uc_currency_sign', '$'))),
    ),
  );

  // Form to select type of rate amount
  $form['uc_fedex_markups']['uc_fedex_rate_markup'] = array(
    '#type'          => 'textfield',
    '#title'         => t('FedEx Shipping Rate Markup'),
    '#default_value' => variable_get('uc_fedex_rate_markup', '0'),
    '#description'   => t('Markup FedEx shipping rate quote by dollar amount, percentage, or multiplier.'),
  );

  // Form to select type of weight markup
  $form['uc_fedex_markups']['uc_fedex_weight_markup_type'] = array(
    '#type'          => 'select',
    '#title'         => t('Weight Markup Type'),
    '#default_value' => variable_get('uc_fedex_weight_markup_type', 'percentage'),
    '#options'       => array(
      'percentage' => t('Percentage (%)'),
      'multiplier' => t('Multiplier (×)'),
      'mass'       => t('Addition (!mass)', array('!mass' => '#')),
    ),
  );

  // Form to select type of weight markup amount
  $form['uc_fedex_markups']['uc_fedex_weight_markup'] = array(
    '#type'          => 'textfield',
    '#title'         => t('FedEx Shipping Weight Markup'),
    '#default_value' => variable_get('uc_fedex_weight_markup', '0'),
    '#description'   => t('Markup FedEx shipping weight before quote by weight amount, percentage, or multiplier.'),
  );

  // Container for validation forms
  $form['uc_fedex_validation'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Address Validation'),
    '#description'   => t('Preferences for FedEx Address Validation.  Additional permissions from FedEx are required to use this feature.'),
    '#collapsible'   => TRUE,
    '#collapsed'     => FALSE,
  );

  // Form to select Residential/Commercial destination address
  $form['uc_fedex_validation']['uc_fedex_address_validation'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Let FedEx determine if an address is Commercial or Residential'),
    '#default_value' => variable_get('uc_fedex_address_validation', FALSE),
    '#description'   => t('When enabled, FedEx Address Validation Web Service will be used to determine if a shipping address is Commercial or Residential.'),
    '#disabled'      => TRUE,
  );

  // Form to select Residential/Commercial destination address
  $form['uc_fedex_validation']['uc_fedex_residential_quotes'] = array(
    '#type'          => 'radios',
    '#title'         => t('Quote rates assuming destination is a'),
    '#default_value' => variable_get('uc_fedex_residential_quotes', 1),
    '#options'       => array(
      0 => t('Commercial address'),
      1 => t('Residential address (extra fees)'),
    ),
    '#description'   => t('This selection will be used if Address Validation is disabled above, or if Address Validation is enabled and fails or times out.'),
  );

  // Form to select Residential/Commercial destination address
  $form['uc_fedex_validation']['uc_fedex_checkout_validation'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Enable checkout address validation'),
    '#default_value' => variable_get('uc_fedex_address_validation', FALSE),
    '#description'   => t('When enabled, JavaScript will be used to validate the shipping address entered by the customer on the checkout page.  Errors will be flagged immediately, before the order is submitted.'),
    '#disabled'      => TRUE,
  );

  // Container for label printing
  $form['uc_fedex_labels'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Label Printing'),
    '#description'   => t('Preferences for FedEx Shipping Label Printing.  Additional permissions from FedEx are required to use this feature.'),
    '#collapsible'   => TRUE,
    '#collapsed'     => FALSE,
  );

  // Form to select label image format
  $form['uc_fedex_labels']['uc_fedex_label_image_format'] = array(
    '#type'          => 'select',
    '#title'         => t('Select image format for shipping labels'),
    '#default_value' => variable_get('uc_fedex_label_image_format', _uc_fedex_label_image_types()),
    '#options'       => _uc_fedex_label_image_types(),
  );

  // Form to select label stock
  $form['uc_fedex_labels']['uc_fedex_label_stock'] = array(
    '#type'          => 'select',
    '#title'         => t('Select Laser or Thermal label stock size'),
    '#default_value' => variable_get('uc_fedex_label_stock', _uc_fedex_label_stock_types()),
    '#options'       => _uc_fedex_label_stock_types(),
  );

  // Form to select label orientation
  $form['uc_fedex_labels']['uc_fedex_label_orientation'] = array(
    '#type'          => 'select',
    '#title'         => t('Select orientation for label printing'),
    '#default_value' => variable_get('uc_fedex_label_orientation', _uc_fedex_label_orientation_types()),
    '#options'       => _uc_fedex_label_orientation_types(),
  );

  // Register validation handler
  $form['#validate'][] = 'uc_fedex_admin_settings_validate';

  return system_settings_form($form);
}


/**
 * Validation handler for uc_fedex_admin_settings form.
 *
 * Require password only if it hasn't been set.
 *
 */
function uc_fedex_admin_settings_validate($form, &$form_state) {

  $old_password = variable_get('uc_fedex_user_credential_password', '');
  if (!$form_state['values']['uc_fedex_user_credential_password']) {
    if ($old_password) {
      form_set_value($form['uc_fedex_credentials']['uc_fedex_user_credential_password'], $old_password, $form_state);
    }
    else {
      form_set_error('uc_fedex_user_credential_password', t('Password field is required.'));
    }
  }
}


/**
 * Last chance for user to review shipment.
 *
 * @ingroup forms
 * @see theme_uc_fedex_confirm_shipment()
 * @see uc_fedex_confirm_shipment_submit()
 */
function uc_fedex_confirm_shipment($order) {
  $form = array();
  $form['digest'] = array(
    '#type'  => 'hidden',
    '#value' => $_SESSION['fedex']['digest']
  );
  $form['submit'] = array(
    '#type'  => 'submit',
    '#value' => t('Generate Label')
  );
  return $form;
}


/**
 * Display final shipment information for review.
 *
 * @ingroup themeable
 * @see uc_fedex_confirm_shipment()
 */
function theme_uc_fedex_confirm_shipment($form) {
  $output = '';

  $output .= '<div class="shipping-address"><b>'. t('Ship from:') .'</b><br />';
  $output .= uc_address_format(
    check_plain($_SESSION['fedex']['origin']->first_name),
    check_plain($_SESSION['fedex']['origin']->last_name),
    check_plain($_SESSION['fedex']['origin']->company),
    check_plain($_SESSION['fedex']['origin']->street1),
    check_plain($_SESSION['fedex']['origin']->street2),
    check_plain($_SESSION['fedex']['origin']->city),
    check_plain($_SESSION['fedex']['origin']->zone),
    check_plain($_SESSION['fedex']['origin']->postal_code),
    check_plain($_SESSION['fedex']['origin']->country)
  );
  $output .= '<br />'. check_plain($_SESSION['fedex']['origin']->email);
  $output .= '</div>';

  $output .= '<div class="shipping-address"><b>'. t('Ship to:') .'</b><br />';
  $output .= uc_address_format(
    check_plain($_SESSION['fedex']['destination']->first_name),
    check_plain($_SESSION['fedex']['destination']->last_name),
    check_plain($_SESSION['fedex']['destination']->company),
    check_plain($_SESSION['fedex']['destination']->street1),
    check_plain($_SESSION['fedex']['destination']->street2),
    check_plain($_SESSION['fedex']['destination']->city),
    check_plain($_SESSION['fedex']['destination']->zone),
    check_plain($_SESSION['fedex']['destination']->postal_code),
    check_plain($_SESSION['fedex']['destination']->country)
  );
  $output .= '<br />'. check_plain($_SESSION['fedex']['destination']->email);
  $output .= '</div>';
  $output .= '<div class="shipment-data">';
  $method = uc_fedex_shipping_method();
  $output .= '<b>'. $method['fedex']['quote']['accessorials'][$_SESSION['fedex']['service']] .'</b><br />';
  $context = array(
    'revision' => 'themed',
    'type'     => 'amount',
  );
  $output .= '<i>'. check_plain($_SESSION['fedex']['rate']['type']) .'</i>: '. uc_price($_SESSION['fedex']['rate']['amount'], $context) .' ('. check_plain($_SESSION['fedex']['rate']['currency']) .')<br />';
  $ship_date = $_SESSION['fedex']['ship_date'];
  $output .= 'Ship date: '. format_date(gmmktime(12, 0, 0, $ship_date['month'], $ship_date['day'], $ship_date['year']), 'custom', variable_get('uc_date_format_default', 'm/d/Y'));
  $exp_delivery = $_SESSION['fedex']['expected_delivery'];
  $output .= '<br />Expected delivery: '. format_date(gmmktime(12, 0, 0, $exp_delivery['month'], $exp_delivery['day'], $exp_delivery['year']), 'custom', variable_get('uc_date_format_default', 'm/d/Y'));
  $output .= "</div>\n<br style=\"clear: both;\" />";
  $output .= drupal_render($form);

  return $output;
}


/**
 * Generate label.
 *
 * @see uc_fedex_confirm_shipment()
 */
function uc_fedex_confirm_shipment_submit($form, &$form_state) {
  // Request pickup using parameters in form.
  $order_id    = $_SESSION['fedex']['order_id'];
  $packages    = $_SESSION['fedex']['packages'];
  $origin      = $_SESSION['fedex']['origin'];
  $destination = $_SESSION['fedex']['destination'];

  //
  // Assemble the label request and send it off to FedEx
  //
  $response = uc_fedex_shipment_request($_SESSION['fedex']['packages'], $origin, $destination, $form_state['values']['service']);

  //
  // Label
  //
  $label = $response->CompletedShipmentDetail->CompletedPackageDetails->Label->Parts->Image;
  //drupal_set_message('<pre>Service:'. var_export($label, TRUE) .'</pre>');

  //
  // Tracking Number
  //
  $tracking = $response->CompletedShipmentDetail->CompletedPackageDetails->TrackingIds->TrackingNumber;
  drupal_set_message('<pre>Tracking#:'. var_export($tracking, TRUE) .'</pre>');

  //
  // Shipping Charge
  //
  $charge = $response->CompletedShipmentDetail->ShipmentRating->ShipmentRateDetails->TotalNetCharge;
  drupal_set_message('<pre>Charge:'. var_export($charge, TRUE) .'</pre>');


  $shipment = new stdClass();
  $shipment->order_id = $order_id;
  $shipment->origin = drupal_clone($_SESSION['fedex']['origin']);
  $shipment->destination = drupal_clone($_SESSION['fedex']['destination']);
  $shipment->packages = $_SESSION['fedex']['packages'];
  $shipment->shipping_method = 'fedex';
  $shipment->accessorials = $_SESSION['fedex']['service'];
  $shipment->carrier = t('FedEx');
  // if NegotiatedRates exist, quote based on those, otherwise, use TotalCharges
  if (isset($response->ShipmentResults->ShipmentCharges)) {
    $charge = $response->ShipmentResults->ShipmentCharges->TotalCharges;
    if (isset($response->ShipmentResults->NegotiatedRates)) {
      $charge = $response->ShipmentResults->NegotiatedRates->NetSummaryCharges->GrandTotal;
    }
  }
  $cost = (string)$charge->MonetaryValue;
  $shipment->cost = $cost;
  $shipment->tracking_number = (string)$response->ShipmentResults->ShipmentIdentificationNumber;
  $ship_date = $_SESSION['fedex']['ship_date'];
  $shipment->ship_date = gmmktime(12, 0, 0, $ship_date['month'], $ship_date['day'], $ship_date['year']);
  $exp_delivery = $_SESSION['fedex']['expected_delivery'];
  $shipment->expected_delivery = gmmktime(12, 0, 0, $exp_delivery['month'], $exp_delivery['day'], $exp_delivery['year']);

  foreach ($response->ShipmentResults->PackageResults as $package_results) {
    $package =& current($shipment->packages);
    $package->tracking_number = (string)$package_results->TrackingNumber;
    $label_image = (string)$package_results->LabelImage->GraphicImage;
    if (file_check_directory(file_create_path('fedex_labels'), FILE_CREATE_DIRECTORY)) {
      $label_path = file_create_path('fedex_labels') .'/label'. $package->tracking_number .'.gif';
      if ($label_file = fopen($label_path, 'wb')) {
        fwrite($label_file, base64_decode($label_image));
        fclose($label_file);
        $package->label_image = $label_path;
      }
      else {
        drupal_set_message(t('Could not open a file to save the label image.'), 'error');
      }
    }
    else {
      drupal_set_message(t('Could not find or create the directory "fedex_labels" in the file system path.'), 'error');
    }
    unset($package);
    next($shipment->packages);
  }

  uc_shipping_shipment_save($shipment);

  unset($_SESSION['fedex']);
  $form_state['redirect'] = 'admin/store/orders/'. $order_id .'/shipments';
}


/**
 * Display the shipping label for printing.
 *
 * Each argument is a component of the file path to the image.
 *
 * @ingroup themeable
 */
function theme_uc_fedex_label_image() {
  $args = func_get_args();
  $image_path = implode('/', $args);
  echo '<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML 3.2//EN">';
  echo '<html><head><title>View/Print Label</title></head>';
  echo '<style>.small-text {font-size: 80%;} .large-text {font-size: 115%;}</style>';
  echo '<body bgcolor="#FFFFFF">';

  $header  = array();
  $rows    = array();

  $title   = 'View/Print Label';
  $items   = array();
  $items[] = '<b>Print the label:</b> Select Print from the File menu in this browser window to print the label below.';
  $items[] = '<b> Fold the printed label at the dotted line.</b> Place the label in a FedEx Label Pouch. If you do not have a pouch, affix the folded label using clear plastic shipping tape over the entire label.';
  $items[] = '<b>GETTING YOUR SHIPMENT TO FedEx</b>';
  $rows[]  = theme('item_list', $items, $title, 'ol');

  $title   = 'Customers without a Daily Pickup';
  $items   = array();
  $items[] = 'Ground, 3 Day Select, and Standard to Canada shipments must be dropped off at an authorized FedEx location, or handed to a FedEx driver. Pickup service is not available for these services. To find the nearest drop-off location, select the Drop-off icon from the FedEx tool bar.';
  $items[] = 'Air shipments (including Worldwide Express and Expedited) can be picked up or dropped off. To schedule a pickup, or to find a drop-off location, select the Pickup or Drop-off icon from the FedEx tool bar.';
  $rows[]  = theme('item_list', $items, $title, 'ul');

  $title   = 'Customers with a Daily Pickup';
  $items   = array();
  $items[] = 'Your driver will pickup your shipment(s) as usual.';
  $rows[]  = theme('item_list', $items, $title, 'ul');

  $rows[]  = array('FOLD HERE', '<hr>');
  $rows[]  = array('&nbsp;');
  $rows[]  = array(theme('image', $image_path, t('FedEx Shipping Label'), array('class' => 'fedex-shipping-label', TRUE)));

  echo theme('table', $header, $rows);
  echo '</body></html>';

  exit();
}
