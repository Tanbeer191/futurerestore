/*
 * Copyright (c) 1998-2014 Apple Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */

/*!
 * @header IOPartitionScheme
 * @abstract
 * This header contains the IOPartitionScheme class definition.
 */

#ifndef _IOPARTITIONSCHEME_H
#define _IOPARTITIONSCHEME_H

/*!
 * @defined kIOPartitionSchemeClass
 * @abstract
 * The name of the IOPartitionScheme class.
 * @discussion
 * kIOPartitionSchemeClass is the name of the IOPartitionScheme class.
 */

#define kIOPartitionSchemeClass "IOPartitionScheme"

/*!
 * @defined kIOMediaBaseKey
 * @abstract
 * A property of IOMedia objects.
 * @discussion
 * The kIOMediaBaseKey property has an OSNumber value and is placed into an
 * IOMedia instance created via the partition scheme. It describes the byte
 * offset of the partition relative to the provider media.
 */

#define kIOMediaBaseKey "Base"

/*!
 * @defined kIOMediaLiveKey
 * @abstract
 * A property of IOMedia objects.
 * @discussion
 * The kIOMediaLiveKey property has an OSBoolean
 * value and is placed into an IOMedia instance
 * created via the partition scheme.  It describes whether the
 * partition is live, that is, it is up-to-date with respect
 * to the on-disk partition table.
 */

#define kIOMediaLiveKey "Live"

/*!
 * @defined kIOMediaPartitionIDKey
 * @abstract
 * A property of IOMedia objects.
 * @discussion
 * The kIOMediaPartitionIDKey property has an OSNumber
 * value and is placed into an IOMedia instance
 * created via the partition scheme.  It is an ID that differentiates one 
 * partition from the other (within a given scheme).  It is typically an index
 * into the on-disk partition table.
 */

#define kIOMediaPartitionIDKey "Partition ID"

#ifdef KERNEL
#ifdef __cplusplus

/*
 * Kernel
 */

#include <IOKit/storage/IOMedia.h>
#include <IOKit/storage/IOStorage.h>

/*!
 * @class IOPartitionScheme
 * @abstract
 * The common base class for all partition scheme
 * objects.
 * @discussion
 * The IOPartitionScheme class is the common base class for all partition scheme
 * objects.  It extends the IOStorage class by implementing the appropriate open
 * and close semantics for partition objects (standard semantics are to act as a
 * multiplexor for incoming opens,  producing one outgoing open with the correct
 * access).  It also implements the default read and write semantics, which pass
 * all reads and writes through to the provider media unprocessed.    For simple
 * schemes, the default behavior is sufficient.   More complex partition schemes
 * such as RAID will want to do extra processing for reads and writes.
 */

class IOPartitionScheme : public IOStorage
{
    OSDeclareDefaultStructors(IOPartitionScheme);

protected:

    struct ExpansionData { /* */ };
    ExpansionData * _expansionData;

    IOStorageAccess _openLevel;
    OSSet *         _openReaders;
    OSSet *         _openReaderWriters;

    /*
     * Free all of this object's outstanding resources.
     */

    virtual void free() APPLE_KEXT_OVERRIDE;

    /*!
     * @function handleOpen
     * @discussion
     * The handleOpen method grants or denies permission to access this object
     * to an interested client.  The argument is an IOStorageAccess value that
     * specifies the level of access desired -- reader or reader-writer.
     *
     * This method can be invoked to upgrade or downgrade the access level for
     * an existing client as well.  The previous access level will prevail for
     * upgrades that fail, of course.   A downgrade should never fail.  If the
     * new access level should be the same as the old for a given client, this
     * method will do nothing and return success.  In all cases, one, singular
     * close-per-client is expected for all opens-per-client received.
     *
     * This implementation replaces the IOService definition of handleOpen().
     * @param client
     * Client requesting the open.
     * @param options
     * Options for the open.  Set to zero.
     * @param access
     * Access level for the open.  Set to kIOStorageAccessReader or
     * kIOStorageAccessReaderWriter.
     * @result
     * Returns true if the open was successful, false otherwise.
     */

    virtual bool handleOpen(IOService *  client,
                            IOOptionBits options,
                            void *       access) APPLE_KEXT_OVERRIDE;

    /*!
     * @function handleIsOpen
     * @discussion
     * The handleIsOpen method determines whether the specified client, or any
     * client if none is specified, presently has an open on this object.
     *
     * This implementation replaces the IOService definition of handleIsOpen().
     * @param client
     * Client to check the open state of.  Set to zero to check the open state
     * of all clients.
     * @result
     * Returns true if the client was (or clients were) open, false otherwise.
     */

    virtual bool handleIsOpen(const IOService * client) const APPLE_KEXT_OVERRIDE;

    /*!
     * @function handleClose
     * @discussion
     * The handleClose method closes the client's access to this object.
     *
     * This implementation replaces the IOService definition of handleClose().
     * @param client
     * Client requesting the close.
     * @param options
     * Options for the close.  Set to zero.
     */

    virtual void handleClose(IOService * client, IOOptionBits options) APPLE_KEXT_OVERRIDE;

    /*
     * Attach the given media object to the device tree plane.
     */

    virtual bool attachMediaObjectToDeviceTree(IOMedia * media);

    /*
     * Detach the given media object from the device tree plane.
     */

    virtual void detachMediaObjectFromDeviceTree(IOMedia * media);

    /*
     * Updates a set of existing partitions, represented by partitionsOld,
     * with possible updates from a rescan of the disk, represented by
     * partitionsNew.  It returns a new set of partitions with the results,
     * removing partitions from partitionsOld where applicable, adding
     * partitions from partitionsNew where applicable, and folding in property
     * changes to partitions from partitionsNew into partitionsOld where
     * applicable.
     */

    virtual OSSet * juxtaposeMediaObjects(OSSet * partitionsOld,
                                          OSSet * partitionsNew);

public:

    using IOStorage::read;
    using IOStorage::write;

    /*
     * Initialize this object's minimal state.
     */

    virtual bool init(OSDictionary * properties = 0) APPLE_KEXT_OVERRIDE;

    /*!
     * @function read
     * @discussion
     * Read data from the storage object at the specified byte offset into the
     * specified buffer, asynchronously.   When the read completes, the caller
     * will be notified via the specified completion action.
     *
     * The buffer will be retained for the duration of the read.
     *
     * For simple partition schemes, the default behavior is to simply pass the
     * read through to the provider media.  More complex partition schemes such
     * as RAID will need to do extra processing here.
     * @param client
     * Client requesting the read.
     * @param byteStart
     * Starting byte offset for the data transfer.
     * @param buffer
     * Buffer for the data transfer.  The size of the buffer implies the size of
     * the data transfer.
     * @param attributes
     * Attributes of the data transfer.  See IOStorageAttributes.  It is the
     * responsibility of the callee to maintain the information for the duration
     * of the data transfer, as necessary.
     * @param completion
     * Completion routine to call once the data transfer is complete.  It is the
     * responsibility of the callee to maintain the information for the duration
     * of the data transfer, as necessary.
     */

    virtual void read(IOService *           client,
                      UInt64                byteStart,
                      IOMemoryDescriptor *  buffer,
                      IOStorageAttributes * attributes,
                      IOStorageCompletion * completion) APPLE_KEXT_OVERRIDE;

    /*!
     * @function write
     * @discussion
     * Write data into the storage object at the specified byte offset from the
     * specified buffer, asynchronously.   When the write completes, the caller
     * will be notified via the specified completion action.
     *
     * The buffer will be retained for the duration of the write.
     *
     * For simple partition schemes, the default behavior is to simply pass the
     * write through to the provider media. More complex partition schemes such
     * as RAID will need to do extra processing here.
     * @param client
     * Client requesting the write.
     * @param byteStart
     * Starting byte offset for the data transfer.
     * @param buffer
     * Buffer for the data transfer.  The size of the buffer implies the size of
     * the data transfer.
     * @param attributes
     * Attributes of the data transfer.  See IOStorageAttributes.  It is the
     * responsibility of the callee to maintain the information for the duration
     * of the data transfer, as necessary.
     * @param completion
     * Completion routine to call once the data transfer is complete.  It is the
     * responsibility of the callee to maintain the information for the duration
     * of the data transfer, as necessary.
     */

    virtual void write(IOService *           client,
                       UInt64                byteStart,
                       IOMemoryDescriptor *  buffer,
                       IOStorageAttributes * attributes,
                       IOStorageCompletion * completion) APPLE_KEXT_OVERRIDE;

    /*!
     * @function synchronize
     * @discussion
     * Flush the cached data in the storage object, if any.
     * @param client
     * Client requesting the synchronization.
     * @param byteStart
     * Starting byte offset for the synchronization.
     * @param byteCount
     * Size of the synchronization.  Set to zero to specify the end-of-media.
     * @param options
     * Options for the synchronization.  See IOStorageSynchronizeOptions.
     * @result
     * Returns the status of the synchronization.
     */

    virtual IOReturn synchronize(IOService *                 client,
                                 UInt64                      byteStart,
                                 UInt64                      byteCount,
                                 IOStorageSynchronizeOptions options = 0) APPLE_KEXT_OVERRIDE;

    /*!
     * @function unmap
     * @discussion
     * Delete unused data from the storage object at the specified byte offsets.
     * @param client
     * Client requesting the operation.
     * @param extents
     * List of extents.  See IOStorageExtent.  It is legal for the callee to
     * overwrite the contents of this buffer in order to satisfy the request.
     * @param extentsCount
     * Number of extents.
     * @param options
     * Options for the unmap.  See IOStorageUnmapOptions.
     * @result
     * Returns the status of the operation.
     */

    virtual IOReturn unmap(IOService *           client,
                           IOStorageExtent *     extents,
                           UInt32                extentsCount,
                           IOStorageUnmapOptions options = 0) APPLE_KEXT_OVERRIDE;

    /*!
     * @function getProvisionStatus
     * @discussion
     * Get device block provision status
     * @param client
     * Client requesting the synchronization.
     * @param byteStart
     * Byte offset of logical extent on the device.
     * @param byteCount
     * Byte length of logical extent on the device, 0 mean the entire remaining space.
     * @param extentsCount
     * Number of extents allocated in extents. On return, this parameter indicate number
     * of provision extents returned.
     * @param extents
     * List of provision extents. See IOStorageProvisionExtents.
     * @result
     * Returns the status of the getProvisionStatus.
     */

    virtual IOReturn getProvisionStatus(IOService *                         client,
                                        UInt64                              byteStart,
                                        UInt64                              byteCount,
                                        UInt32 *                            extentsCount,
                                        IOStorageProvisionExtent *          extents,
                                        IOStorageGetProvisionStatusOptions  options = 0) APPLE_KEXT_OVERRIDE;

    /*!
     * @function lockPhysicalExtents
     * @discussion
     * Lock the contents of the storage object against relocation temporarily,
     * for the purpose of getting physical extents.
     * @param client
     * Client requesting the operation.
     * @result
     * Returns true if the lock was successful, false otherwise.
     */

    virtual bool lockPhysicalExtents(IOService * client) APPLE_KEXT_OVERRIDE;

    /*!
     * @function copyPhysicalExtent
     * @discussion
     * Convert the specified byte offset into a physical byte offset, relative
     * to a physical storage object.  This call should only be made within the
     * context of lockPhysicalExtents().
     * @param client
     * Client requesting the operation.
     * @param byteStart
     * Starting byte offset for the operation.  Returns a physical byte offset,
     * relative to the physical storage object, on success.
     * @param byteCount
     * Size of the operation.  Returns the actual number of bytes which can be
     * transferred, relative to the physical storage object, on success. 
     * @result
     * A reference to the physical storage object, which should be released by
     * the caller, or a null on error.
     */

    virtual IOStorage * copyPhysicalExtent(IOService * client,
                                           UInt64 *    byteStart,
                                           UInt64 *    byteCount) APPLE_KEXT_OVERRIDE;

    /*!
     * @function unlockPhysicalExtents
     * @discussion
     * Unlock the contents of the storage object for relocation again.  This
     * call must balance a successful call to lockPhysicalExtents().
     * @param client
     * Client requesting the operation.
     */

    virtual void unlockPhysicalExtents(IOService * client) APPLE_KEXT_OVERRIDE;

    /*!
     * @function setPriority
     * @discussion
     * Reprioritize read or write requests at the specified byte offsets.
     * @param client
     * Client requesting the operation.
     * @param extents
     * List of extents.  See IOStorageExtent.  It is legal for the callee to
     * overwrite the contents of this buffer in order to satisfy the request.
     * @param extentsCount
     * Number of extents.
     * @param priority
     * New priority.  See IOStoragePriority.
     * @result
     * Returns the status of the operation.
     */

    virtual IOReturn setPriority(IOService *       client,
                                 IOStorageExtent * extents,
                                 UInt32            extentsCount,
                                 IOStoragePriority priority) APPLE_KEXT_OVERRIDE;

    /*
     * Obtain this object's provider.  We override the superclass's method
     * to return a more specific subclass of OSObject -- an IOMedia.  This
     * method serves simply as a convenience to subclass developers.
     */

    virtual IOMedia * getProvider() const APPLE_KEXT_OVERRIDE;

    OSMetaClassDeclareReservedUnused(IOPartitionScheme,  0);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme,  1);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme,  2);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme,  3);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme,  4);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme,  5);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme,  6);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme,  7);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme,  8);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme,  9);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme, 10);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme, 11);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme, 12);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme, 13);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme, 14);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme, 15);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme, 16);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme, 17);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme, 18);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme, 19);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme, 20);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme, 21);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme, 22);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme, 23);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme, 24);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme, 25);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme, 26);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme, 27);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme, 28);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme, 29);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme, 30);
    OSMetaClassDeclareReservedUnused(IOPartitionScheme, 31);
};

#ifdef KERNEL_PRIVATE
#define kIOPartitionScheme_partition_valid      ( ( intptr_t ) 1 )
#define _partitionSchemeState                   ( *( intptr_t *) &( IOPartitionScheme::_expansionData ) )
#endif /* KERNEL_PRIVATE */

#endif /* __cplusplus */
#endif /* KERNEL */
#endif /* !_IOPARTITIONSCHEME_H */
