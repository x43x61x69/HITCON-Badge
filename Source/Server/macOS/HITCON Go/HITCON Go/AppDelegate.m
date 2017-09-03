//
//  AppDelegate.m
//  HITCON Go
//
//  Copyright (C) 2017 Zhi-Wei Cai.
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifdef DEBUG
#define LOGD(fmt, ...) NSLog(@"[LOG] %s " fmt "", __FUNCTION__ , ## __VA_ARGS__);
#else
#define LOGD(...)
#endif

#define CONST_TOKEN_SALT                @"_MY_AWESOME_HITCON_TOKEN_"
#define TOKEN_DELAY                     5
#define TOKEN_LENGTH                    8
#define TOKEN_DATA                      [[self token] dataUsingEncoding:NSUTF8StringEncoding]
#define CONST_HW_NAME                   [[[NSUUID UUID] UUIDString] substringToIndex:8]
#define CONST_HW_ADVERT_UUID            @"45AE269D-C1A1-4F75-A4C6-E33E8D066CD9"
#define CONST_HW_SERVICE_UUID           @"45AE269D-C1A1-4F75-A4C6-E33E8D066CD9"
#define CONST_HW_CHARACTERISTIC_UUID    @"F01BE275-89E6-4D88-ABA2-94483C952887"

#import "AppDelegate.h"
#import <CoreBluetooth/CoreBluetooth.h>
#import <CommonCrypto/CommonDigest.h>

@implementation NSData (Extension)

- (NSString *)md5
{
    unsigned char buf[CC_MD5_DIGEST_LENGTH];
    CC_MD5(self.bytes, (CC_LONG)self.length, buf);
    NSMutableString *result = [NSMutableString stringWithCapacity:CC_MD5_DIGEST_LENGTH * 2];
    for (int i = 0; i < CC_MD5_DIGEST_LENGTH; i++) {
        [result appendFormat:@"%02x", buf[i]];
    }
    return result;
}

@end

@interface AppDelegate () <CBPeripheralManagerDelegate>
{
    NSTimer *renewTimer;
}

@property (nonatomic, strong) CBPeripheralManager           *manager;
@property (nonatomic, strong) CBMutableService              *service;
@property (nonatomic, strong) NSMutableArray<CBCentral *>   *centrals;
@property (nonatomic, strong) CBMutableCharacteristic       *characteristic;

@property (weak) IBOutlet NSWindow  *window;
@property (weak) IBOutlet NSButton  *powerButton;
@property (weak) IBOutlet NSButton  *tamperSwitch;

@property (unsafe_unretained) IBOutlet NSTextView *logView;

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    [self newMessage:[NSString stringWithFormat:@"%@ Version: %@",
                      [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleName"],
                      [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"]]];
    [self newMessage:[NSString stringWithFormat:@"%@",
                      [[[NSBundle mainBundle] infoDictionary] objectForKey:@"NSHumanReadableCopyright"]]];
    
    [self newMessage:@"PLEASE MAKE SURE YOUR LOCAL TIME IS ALIGNED WITH THE KEY SERVER!"];
    
    _manager = [[CBPeripheralManager alloc] initWithDelegate:self
                                                       queue:nil
                                                     options:@{CBPeripheralManagerOptionShowPowerAlertKey : @YES}];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification
{
    [_manager stopAdvertising];
    [_manager removeAllServices];
    _manager = nil;
}

#pragma mark - CBPeripheralManagerDelegate

- (void)peripheralManagerDidUpdateState:(CBPeripheralManager *)peripheral
{
    LOGD(@"peripheralManagerDidUpdateState: %d", (int)peripheral.state);
    
    if (peripheral.state == CBPeripheralManagerStatePoweredOn) {
        _powerButton.enabled = YES;
        [self newMessage:@"Bluetooth is ready."];
    } else {
        _powerButton.enabled = NO;
        [self newMessage:@"Bluetooth is unavailable."];
        [self powerSwitch:self];
    }
}

- (void)peripheralManagerDidStartAdvertising:(CBPeripheralManager *)peripheral error:(NSError *)error
{
    LOGD(@"peripheralManagerDidStartAdvertising: %@", error);
    
    if (error) {
        [self newMessage:[error localizedDescription]];
    }
}

- (void)peripheralManager:(CBPeripheralManager *)peripheral didAddService:(CBService *)service error:(NSError *)error
{
    if (error) {
        [self newMessage:[error localizedDescription]];
    }
    
    //[self newMessage:[NSString stringWithFormat:@"Added Service: %@", [[service UUID] UUIDString]]];
}

- (void)peripheralManager:(CBPeripheralManager *)peripheral central:(CBCentral *)central didSubscribeToCharacteristic:(CBCharacteristic *)characteristic
{
    
    [_centrals addObject:central];
    
    [self newMessage:[NSString stringWithFormat:@"New Subscription to \"%@\": %@",
                      [characteristic UUID],
                      [central.identifier UUIDString]]];
}

- (void)peripheralManager:(CBPeripheralManager *)peripheral didReceiveReadRequest:(CBATTRequest *)request
{
    if (request.characteristic.properties &
        CBCharacteristicPropertyRead) {
        
        //[request setValue:request.characteristic.value];
        
        [request setValue:TOKEN_DATA];
        
        [peripheral respondToRequest:request
                          withResult:CBATTErrorSuccess];
        
    } else {
        
        [peripheral respondToRequest:request
                          withResult:CBATTErrorWriteNotPermitted];
    }
    
    [self newMessage:[NSString stringWithFormat:@"New Read Request: %@",
                      [request value]]];
}

- (void)peripheralManager:(CBPeripheralManager *)peripheral didReceiveWriteRequests:(NSArray<CBATTRequest *> *)requests
{
    for (CBATTRequest *request in requests) {
        [self newMessage:[NSString stringWithFormat:@"New Write Request: %@",
                          [request value]]];
    }
}

#pragma mark - Methods

- (BOOL)isReady
{
    return CBPeripheralManagerStatePoweredOn == _manager.state && _manager.isAdvertising;
}

#pragma mark - IBOutlets

- (void)newMessage:(NSString *)msg
{
    NSDateFormatter *dateFormat = [[NSDateFormatter alloc] init];
    dateFormat.calendar = [NSCalendar calendarWithIdentifier:NSCalendarIdentifierISO8601];
    [dateFormat setDateFormat:@"yyyy-MM-dd HH:mm:ss"];
    _logView.string = [NSString stringWithFormat:@"[%@] %@\n%@",
                       [dateFormat stringFromDate:[NSDate date]],
                       msg,
                       _logView.string];
}

- (IBAction)clearMessage:(id)sender
{
    _logView.string = @"";
}

- (IBAction)powerSwitch:(id)sender
{
    if (CBPeripheralManagerStatePoweredOn == _manager.state &&
        _manager.isAdvertising == NO) {
        
        _powerButton.title = @"Power Off";
        [self newMessage:@"Power On"];
        
        [self renew];
        
    } else {
        
        [_manager stopAdvertising];
        [_manager removeAllServices];
        
        if ([renewTimer isValid]) {
            [renewTimer invalidate];
        }
        
        _powerButton.title = @"Power On";
        [self newMessage:@"Power Off"];
    }
}

- (IBAction)tamper:(id)sender
{
    if ([_tamperSwitch state] == NSOnState) {
        
        [self newMessage:@"[WARNING] Tampering is ON!"];
        
    } else {
        
        [self newMessage:@"[WARNING] Tampering is OFF!"];
    }
    
    if (CBPeripheralManagerStatePoweredOn == _manager.state &&
        _manager.isAdvertising == YES) {
        
        [self renew];
        
    }
}

- (void)renew
{
    [self newMessage:@"Renew Token"];
    
    [_manager stopAdvertising];
    [_manager removeAllServices];
    
    NSString *newDevice = CONST_HW_NAME;
    
    [self newMessage:[NSString stringWithFormat:@"New Device Name: %@",
                      newDevice]];
    
    [_manager startAdvertising:@{CBAdvertisementDataLocalNameKey    :newDevice,
                                 CBAdvertisementDataServiceUUIDsKey :@[[CBUUID UUIDWithString:CONST_HW_ADVERT_UUID]]}];
    _characteristic = [[CBMutableCharacteristic alloc] initWithType:[CBUUID UUIDWithString:CONST_HW_CHARACTERISTIC_UUID]
                                                         properties:CBCharacteristicPropertyRead
                                                              value:TOKEN_DATA
                                                        permissions:CBAttributePermissionsReadable];
    
    _service = [[CBMutableService alloc] initWithType:[CBUUID UUIDWithString:CONST_HW_SERVICE_UUID]
                                              primary:YES];
    _service.characteristics = @[_characteristic];
    [_manager addService:_service];
}

- (NSString *)token
{
    NSCalendar *calendar = [[NSCalendar alloc] initWithCalendarIdentifier:NSCalendarIdentifierISO8601];
    NSDate *currentDate = [NSDate date];
    
    NSDateComponents *components = [calendar components:(NSCalendarUnitMonth|
                                                         NSCalendarUnitDay|
                                                         NSCalendarUnitYear|
                                                         NSCalendarUnitHour|
                                                         NSCalendarUnitMinute)
                                               fromDate:currentDate];
    currentDate = [calendar dateFromComponents:components];
    
    LOGD(@"currentDate: %@", currentDate);
    NSInteger newMinute = components.minute / TOKEN_DELAY * TOKEN_DELAY - components.minute;
    LOGD(@"newMinute: %ld", (long)newMinute);
    components = [NSDateComponents new];
    [components setMinute:newMinute];
    NSDate *fixedDate = [calendar dateByAddingComponents:components
                                                  toDate:currentDate
                                                 options:0];
    LOGD(@"fixedDate: %@", fixedDate);
    
    if ([renewTimer isValid]) {
        [renewTimer invalidate];
    }
    
    renewTimer = [NSTimer scheduledTimerWithTimeInterval:(TOKEN_DELAY + newMinute) * 60
                                                  target:self
                                                selector:@selector(renew)
                                                userInfo:nil
                                                 repeats:NO];
    
    NSDateFormatter *dateFormat = [NSDateFormatter new];
    dateFormat.calendar = [NSCalendar calendarWithIdentifier:NSCalendarIdentifierISO8601];
    [dateFormat setDateFormat:@"yyyy-MM-dd HH:mm:ss"];
    [dateFormat setTimeZone:[NSTimeZone timeZoneWithAbbreviation:@"GMT"]];
    NSString *dateString = [dateFormat stringFromDate:fixedDate];
    NSString *tokenString = [NSString stringWithFormat:@"%@%@",
                             CONST_TOKEN_SALT,
                             dateString];
    NSString *tokenHash = [[[tokenString dataUsingEncoding:NSUTF8StringEncoding]
                            md5]
                           substringWithRange:NSMakeRange(5, TOKEN_LENGTH - 1)];
    
    int checksum = 0;
    for (int i = 0; i < TOKEN_LENGTH - 1; i++) {
        NSString *c = [tokenHash substringWithRange:NSMakeRange(i, 1)];
        
        unsigned value = 0;
        NSScanner *scanner = [NSScanner scannerWithString:c];
        
        [scanner setScanLocation:0];
        [scanner scanHexInt:&value];
        
        checksum += value * (i + 13);
    }
    checksum %= 16;
    
    tokenHash = [tokenHash stringByAppendingFormat:@"%x", checksum];
    
    NSMutableString *scrambledHash = [NSMutableString new];
    
    for (int i = 0; i < TOKEN_LENGTH; i++) {
        NSString *c = [tokenHash substringWithRange:NSMakeRange(i, 1)];
        
        unsigned value = 0;
        NSScanner *scanner = [NSScanner scannerWithString:c];
        
        [scanner setScanLocation:0];
        [scanner scanHexInt:&value];
        
        LOGD(@"%@ (%d * (%d + 1337) %% 16)", c, value, i);
        
        [scrambledHash appendString:[NSString stringWithFormat:@"%x",
                                     value * (i + 1337) % 16]];
    }
    
    [self newMessage:[NSString stringWithFormat:@"New Token: %@ -> %@ (%@)", tokenHash, scrambledHash, dateString]];
    
    LOGD(@"Token Date: %@",         dateString);
    LOGD(@"Token String: %@",       tokenString);
    LOGD(@"Token Hash: %@ -> %@",   tokenHash, scrambledHash);
    
    if ([_tamperSwitch state] == NSOnState) {
        [self newMessage:@"[WARNING] Tampering token..."];
        return [NSString stringWithFormat:@"k%@_/k", tokenHash];
    }
    
    return [NSString stringWithFormat:@"k%@/k", tokenHash];
}

@end
