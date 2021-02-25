/*
 * Copyright (C) 2004-2016 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#import <WebKit/DOMHTMLElement.h>

@class DOMFileList;
@class DOMHTMLFormElement;
@class NSString;
@class NSURL;

NS_CLASS_AVAILABLE_MAC(10_4)
@interface DOMHTMLInputElement : DOMHTMLElement
@property (copy) NSString *accept;
@property (copy) NSString *alt;
@property BOOL autofocus NS_AVAILABLE_MAC(10_6);
@property BOOL defaultChecked;
@property BOOL checked;
@property BOOL disabled;
@property (readonly, strong) DOMHTMLFormElement *form;
@property (strong) DOMFileList *files NS_AVAILABLE_MAC(10_6);
@property BOOL indeterminate NS_AVAILABLE_MAC(10_5);
@property int maxLength;
@property BOOL multiple NS_AVAILABLE_MAC(10_6);
@property (copy) NSString *name;
@property BOOL readOnly;
@property (copy) NSString *size;
@property (copy) NSString *src;
@property (copy) NSString *type;
@property (copy) NSString *defaultValue;
@property (copy) NSString *value;
@property (readonly) BOOL willValidate NS_AVAILABLE_MAC(10_6);
@property int selectionStart NS_AVAILABLE_MAC(10_5);
@property int selectionEnd NS_AVAILABLE_MAC(10_5);
@property (copy) NSString *align;
@property (copy) NSString *useMap;
@property (copy) NSString *accessKey NS_DEPRECATED_MAC(10_4, 10_8);
@property (readonly, copy) NSString *altDisplayString NS_AVAILABLE_MAC(10_5);
@property (readonly, copy) NSURL *absoluteImageURL NS_AVAILABLE_MAC(10_5);

- (void)select;
- (void)setSelectionRange:(int)start end:(int)end NS_AVAILABLE_MAC(10_5);
- (void)click;
@end
