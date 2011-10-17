/**
 * @file
 * Utility functions to display settings summaries on vertical tabs.
 */

(function ($) {

Drupal.behaviors.fedexAdminFieldsetSummaries = {
  attach: function (context) {
    $('fieldset#edit-uc-fedex-credentials', context).drupalSetSummary(function(context) {
      var server = $('#edit-uc-fedex-server-role', context).val();
      return Drupal.t('Using FedEx @role server', { '@role': server });
    });

    $('fieldset#edit-uc-fedex-markups', context).drupalSetSummary(function(context) {
      return Drupal.t('Rate markup') + ': '
        + $('#edit-uc-fedex-rate-markup', context).val() + ' '
        + $('#edit-uc-fedex-rate-markup-type', context).val() + '<br />'
        + Drupal.t('Weight markup') + ': '
        + $('#edit-uc-fedex-weight-markup', context).val() + ' '
        + $('#edit-uc-fedex-weight-markup-type', context).val();
    });

    $('fieldset#edit-uc-fedex-validation', context).drupalSetSummary(function(context) {
      if ($('#edit-uc-fedex-address-validation').is(':checked')) {
        return Drupal.t('Validation is enabled');
      }
      else {
        return Drupal.t('Validation is disabled');
      }
    });

    $('fieldset#edit-uc-fedex-quote-options', context).drupalSetSummary(function(context) {
      if ($('#edit-uc-fedex-insurance').is(':checked')) {
        return Drupal.t('Packages are insured');
      }
      else {
        return Drupal.t('Packages are not insured');
      }
    });
  }
};

})(jQuery);
