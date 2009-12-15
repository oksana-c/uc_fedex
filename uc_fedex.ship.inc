<?php
// $Id$

/**
 * @file
 * FedEx Web Services Rate / Available Services Quote
 *
 * Shipping quote module that interfaces with the FedEx Web Services API
 * to get rates for small package shipments.  Implements a SOAP Web Service
 * client.
 *
 * @author Tim Rohaly.    <http://drupal.org/user/202830>
 * @version $Id$
 */


/******************************************************************************
 * Label Printing (ship service)                                              *
 ******************************************************************************/


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
  $output .= "</div>\n<br style=\"clear: both;\" />";
  $output .= drupal_render($form);

  return $output;
}


/**
 * Display the shipping label for printing.
 *
 * Each argument is a component of the file path to the image.
 *
 * @ingroup themeable
 */
function uc_fedex_label_image() {
  $args = func_get_args();
  $image_path = implode('/', $args);

  drupal_goto($image_path);

  // Alternate strategy for image display:
  // Handle PDF files differently, because they can't be displayed
  // with an <img> tag
  $filename  = explode('.', end($args));
  $extension = $filename[1];
  if ($extension == 'pdf') {
    drupal_goto($image_path);
  }
}


/**
 * Pass final information into shipment object.
 *
 * @see
 *   uc_fedex_fulfill_order()
 *   uc_fedex_confirm_shipment()
 */
function uc_fedex_fulfill_order_submit($form, &$form_state) {
  $form_state['redirect'] = 'admin/store/orders/'. $form_state['values']['order_id'] .'/shipments/fedex';
}
/**
 * Convenience function to get FedEx codes for payor.
 *
 * @return
 *   An array of human-friendly names for the different FedEx payor option codes
 */
function _uc_fedex_payor_types() {
  return array(
    'RECIPIENT'   => t('Shipping billed to Recipient'),
    'SENDER'      => t('Shipping billed to Sender'),
    'THIRD_PARTY' => t('Shipping billed to third party'),
  );
}


/**
 * Convenience function to get FedEx codes for label image format.
 *
 * @return
 *   An array of human-friendly names for the different FedEx label image formats
 */
function _uc_fedex_label_image_types() {
  return array(
    'PNG'   => t('PNG - Portable Next Generation'),
    'PDF'   => t('PDF - Adobe Portable Document Format'),
    'EPL2'  => t('EPL2 - Eltron Thermal Printer Format'),
    'DPL'   => t('DPL - Unimark Thermal Printer Format'),
    'ZPLII' => t('ZPLII - Zebra Thermal Printer Format'),
  );
}


/**
 * Convenience function to get FedEx codes for label orientation.
 *
 * @return
 *   An array of human-friendly names for the different FedEx label orientation
 */
function _uc_fedex_label_orientation_types() {
  return array(
    'BOTTOM_EDGE_OF_TEXT_FIRST' => t('BOTTOM_EDGE_OF_TEXT_FIRST'),
    'TOP_EDGE_OF_TEXT_FIRST'    => t('TOP_EDGE_OF_TEXT_FIRST'),
  );
}


/**
 * Convenience function to get FedEx codes for label paper stock type.
 *
 * @return
 *   An array of human-friendly names for the different FedEx label paper stock types
 */
function _uc_fedex_label_stock_types() {
  return array(
    'PAPER_4X6'                      => t('Laser 4" x 6"'),
    'PAPER_4X8'                      => t('Laser 4" x 8"'),
    'PAPER_4X9'                      => t('Laser 4" x 9"'),
    'PAPER_7X4.75'                   => t('Laser 7" x 4.75"'),
    'PAPER_8.5X11_BOTTOM_HALF_LABEL' => t('Laser 8.5" x 11", Label on Bottom Half'),
    'PAPER_8.5X11_TOP_HALF_LABEL'    => t('Laser 8.5" x 11", Label on Top Half'),
    'STOCK_4X6'                      => t('Thermal 4" x 6"'),
    'STOCK_4X6.75_LEADING_DOC_TAB'   => t('Thermal 4" x 6.75", Leading DocTab'),
    'STOCK_4X6.75_TRAILING_DOC_TAB'  => t('Thermal 4" x 6.75", Trailing DocTab'),
    'STOCK_4X8'                      => t('Thermal 4" x 8"'),
    'STOCK_4X9_LEADING_DOC_TAB'      => t('Thermal 4" x 9", Leading DocTab'),
    'STOCK_4X9_TRAILING_DOC_TAB'     => t('Thermal 4" x 9", Trailing DocTab'),
  );
}
