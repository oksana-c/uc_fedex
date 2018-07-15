**FedEx Shipping Quote Calculation - Notes**

Documenting my findings from testing current implementation of the `uc_fedex` module:
________________
## FedEx module
Module is programmed to work with `small_packages` only. It is not designed for getting FedEx Freight Quotes. Admin interface that has options for selecting FedEx Freight quote calculation is misleading. Maintainer of the module [publicly states](https://www.drupal.org/node/490210) that module does not support FedEx freight as-is, but has a potential to be expanded to support it. Module takes all items in the cart and breaks them down into `packages` that are <= 150 lbs according to `weight` of the items. It does not take into account dimensions of the items at all.

**FedEx rate calculation happens as follows**
>For each shipment, your charge is based on the dimensional weight or actual weight of the package — whichever is greater.*

>*FedEx reserves the right to re-weigh and measure each package to verify the actual weight and dimensional weight provided by the sender. If the dimensional weight exceeds the actual weight, the package will be rated based on dimensional weight and subject to additional charges.

`Dimensional weight` is not covered by the module, but it should be if you need to avoid overcharging your customers.

Taking into account actual **dimensions** of the items is essential, considering that FedEx imposes dimension restrictions that are different for various methods of shipping. Especially because your items may be matching `weight` requirements, but are completely out of range for `dimensions` requirements. See example below:

**1-1/2" X 5-1/2" x 191-1/4" Fence Rail - Almond** 
`length` = 191.25" -> FedEx length limit for non-Freight small_packages is `108"`
`weight` = 10 lbs

When customer adds 10 pieces of this Rail in cart, FedEx module packages all 10 pieces into `1 package` because the weight amounts to less than 150 lbs. It does not give any warning about the length exceeding the limit of `108"` for `packages`. 

Below is the excerpt from the FedEx General Packaging Guidelines - http://images.fedex.com/us/services/pdf/packaging/GrlPkgGuidelines_fxcom.pdf

>`With FedEx Ground® services, you can ship packages
up to 150 lbs.; up to 108" in length and 165" in length
plus girth. `

___________________________

### Freight Shipping (LTL)

**Freight Packaging Guidelines** have their own restrictions as to dimensions and they also differ greatly according to the specific Shipping method selected. Here's the document - http://images.fedex.com/us/services/pdf/FreightPackagingGuidelines.pdf. Please see page 16 of the PDF for exact dimensions.

> ` FedEx Freight® Priority and FedEx Freight® Economy` allow for shipments up to 24 feet in length. Other Freight Shipping methods allow for up to `119"` for the skid/pallete length.

___________________________________
### Some conclusions 
Currently `uc_fedex` requires HUGE amount of modifications/additions in order to accommodate LTL shipping needs. Having it function only on `weight` values of products will produce faulty quotes and will result in issues with FedEx service (wrong labels, administrative issues, etc.)

_______________________
### Objectives
Restructure the code of `uc_fedex` to support:
- Update RateService_v8.wsdl file to the newest one RateService_v20.wsdl to comply with FedEx standards. Make sure that the rest of the code is inline with latest FedEx requirements. FedEx Module was neglected for a long time and many things have changed
- consideration for `dimensional weight`
- honor `dimension` restrictions for various type of shipping methods according to latest standards
- make sure returned shipping rates are accurate according to the shipping method selected
- tweak admin interface in order to hide shipping methods that are not used if we can't cover those with code conditions
- consideration (in code) for which quotes to request (LTL or non-LTL quotes) according to dimensions/weight of the shipment. According to FedEx API - single request for rates can contain only one kind of quotes - Freight or non-Freight. 

### TODO

- Add Insurance for the full value - as per settings page.
- Get Freight Shipper_ID_Number from FedEx
- Get upgraded developer access to test shipping labels live. Can't test without upgraded access. This involves calling FedEx.
- Explain Kevin how to *submit shipping labels to FedEx for approval*, so that FedEx can allow access to that service in production. Involves printing test labels and sending them physically/by mail to FedEx office. This is done to ensure that the application/module is working correctly, embedding proper info into the label and generating barcodes properly.


Nice-to-haves:
- Show user approximate days of delivery for each shipping option.
- If we're going to show the date -then there must be a warning that the date applies from the time the order is submitted to fedex, not from the current date.
______________________

### LTL Required Elements that differ from non-Freight requests

**LineItem/FreightClass** - Required -  https://smallbusiness.fedex.com/ltl-class-codes.html

Tool to define Freight Class of the shipment - https://smallbusiness.fedex.com/freight-classification

CLASS_300 - result from the online tool. Selected material "Metal & Wood Materials", which seemed the most appropriate for the client. Class depends on 1) dimensions, 2) materials, 3) weight

**Tool to calculate LTL Freight rates** https://www.fedex.com/ratefinder/standalone?method=goToFreight&from=Package

_____________________________
### Shipment Dimension Regulations

#### Freight - Pallete/Skid
**Recommended: Standard Wood Pallets**

FedEx Packaging Services prefers the standard wood pallet
developed by the Grocery Manufacturers Association (GMA).
It typically measures 40" by 48" and features four-way entry
capabilities.

**NOTE: We are establishing a unit _SKID_ for LTL Freight shipments, as compared to _PACKAGE_ for non-freight shipments to cover the case when packages exceeding standard pallete dimensions.**

**FedEx Freight® Priority  /  FedEx Freight® Economy** max dimensions

- Maximum weight per piece (skid) - 3,150 lbs. (1,429 kg)
- Maximum length per piece (skid) - 24' (7 m)
- Maximum height per piece (skid) - 106" (269 cm)
- Maximum width per piece (skid) - 93" (236 cm)

##### EXTREME_LENGTH 

__*FedEx Freight Economy / Priority*__ 

FedEx provides standard packaging options for FedEx Freight Priority and FedEx Freight Economy shipments.
Freight max dimensions are as follows:

- Height: 106 inches
- Width: 93 inches
- Length: 179 inches

Note: Anything with a length of *180 inches and greater* is considered *Extreme Length* and would need to be flagged as such within in the SpecialServicesRequested element.

[Source](https://www.fedex.com/us/developer/WebHelp/ws/2015/html/WebServicesHelp/WSDVG/32_FedEx_Freight_Services.htm)

[Find Freight packaging guidelines](fedex.com/us/services/pdf/FreightPackagingGuidelines.pdf)

When shipments contain any shipping unit or piece with a dimension of **15 feet / 180 in** or greater in length, the following charges will apply:
_$85.00 per shipment, in addition to the otherwise applicable rates and charges._

[FedEx Freight Extras from the experts](http://images.fedex.com/us/freight/rulestariff/Optional_and_Additional_Services_Quicksheet.pdf)

###### UPDATES:
Effective January 2, 2017, the FedEx Freight Extreme Length surcharge will change from $85 to $150 and will be applied to shipments with dimensions of 12 feet or greater versus the prior 15 feet.
http://shipwatchers.kayako.com/index.php?/News/NewsItem/View/524

#### FedEx Ground and FedEx Express

General regulations as to dimensions:

- weight - up to 150 lbs.;
- length - up to 108"
- length + girth 165" *girth = 2 x width + 2 x height*

_______________________
### TROUBLESHOOTING / Common Issues

*1) FedEx Ground rate quote is returned, but FedEx Ground Home Delivery is not.*

- FedEx Ground is for commercial shipments, while FedEx Home Delivery is for residential shipments. Home-based businesses do not qualify as a commercial address.
- Weight of the package is more than the service limit - 70lbs


*2) Certain services completely fail to return a quote.*

- The package may exceed FedEx's maximum serviceable length for that particular service. Minor overages will trigger additional handling fees and surcharges as mentioned above, but extreme overages may render the package completely incompatible, thus failing to return a quote at all.
- The destination address or country may fall outside of that particular service's specific service area.
- The origin zip code may be wrong or incomplete. Where possible, use the full nine-digit zip code for your origin address ("shipping from" address) 

_______________________
### UPS shipping module
**UPS** module is designed the same way as FedEx module with the only difference that it is preventing quote to be generated if `dimensions` of the package do not fit the set criteria.

Same amount of work will have to be performed to both modules in order to facilitate calculation of quotes for different shipping methods that are available.

Additional info for UPS LTL can be found here - http://ltl.upsfreight.com/downloads/upgf102J20160919.pdf
