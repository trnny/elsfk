// source: msg.proto
/**
 * @fileoverview
 * @enhanceable
 * @suppress {messageConventions} JS Compiler reports an error if a variable or
 *     field starts with 'MSG_' and isn't a translatable message.
 * @public
 */
// GENERATED CODE -- DO NOT EDIT!

var jspb = require('google-protobuf');
var goog = jspb;
var global = Function('return this')();

goog.exportSymbol('proto.Msg', null, global);
goog.exportSymbol('proto.Msg.VType', null, global);
/**
 * Generated by JsPbCodeGenerator.
 * @param {Array=} opt_data Optional initial data array, typically from a
 * server response, or constructed directly in Javascript. The array is used
 * in place and becomes part of the constructed object. It is not cloned.
 * If no data is provided, the constructed object will be empty, but still
 * valid.
 * @extends {jspb.Message}
 * @constructor
 */
proto.Msg = function(opt_data) {
  jspb.Message.initialize(this, opt_data, 0, -1, null, null);
};
goog.inherits(proto.Msg, jspb.Message);
if (goog.DEBUG && !COMPILED) {
  /**
   * @public
   * @override
   */
  proto.Msg.displayName = 'proto.Msg';
}
/**
 * Generated by JsPbCodeGenerator.
 * @param {Array=} opt_data Optional initial data array, typically from a
 * server response, or constructed directly in Javascript. The array is used
 * in place and becomes part of the constructed object. It is not cloned.
 * If no data is provided, the constructed object will be empty, but still
 * valid.
 * @extends {jspb.Message}
 * @constructor
 */
proto.Msg.VType = function(opt_data) {
  jspb.Message.initialize(this, opt_data, 0, -1, proto.Msg.VType.repeatedFields_, null);
};
goog.inherits(proto.Msg.VType, jspb.Message);
if (goog.DEBUG && !COMPILED) {
  /**
   * @public
   * @override
   */
  proto.Msg.VType.displayName = 'proto.Msg.VType';
}



if (jspb.Message.GENERATE_TO_OBJECT) {
/**
 * Creates an object representation of this proto.
 * Field names that are reserved in JavaScript and will be renamed to pb_name.
 * Optional fields that are not set will be set to undefined.
 * To access a reserved field use, foo.pb_<name>, eg, foo.pb_default.
 * For the list of reserved names please see:
 *     net/proto2/compiler/js/internal/generator.cc#kKeyword.
 * @param {boolean=} opt_includeInstance Deprecated. whether to include the
 *     JSPB instance for transitional soy proto support:
 *     http://goto/soy-param-migration
 * @return {!Object}
 */
proto.Msg.prototype.toObject = function(opt_includeInstance) {
  return proto.Msg.toObject(opt_includeInstance, this);
};


/**
 * Static version of the {@see toObject} method.
 * @param {boolean|undefined} includeInstance Deprecated. Whether to include
 *     the JSPB instance for transitional soy proto support:
 *     http://goto/soy-param-migration
 * @param {!proto.Msg} msg The msg instance to transform.
 * @return {!Object}
 * @suppress {unusedLocalVariables} f is only used for nested messages
 */
proto.Msg.toObject = function(includeInstance, msg) {
  var f, obj = {
    desc: jspb.Message.getFieldWithDefault(msg, 1, ""),
    dataMap: (f = msg.getDataMap()) ? f.toObject(includeInstance, proto.Msg.VType.toObject) : []
  };

  if (includeInstance) {
    obj.$jspbMessageInstance = msg;
  }
  return obj;
};
}


/**
 * Deserializes binary data (in protobuf wire format).
 * @param {jspb.ByteSource} bytes The bytes to deserialize.
 * @return {!proto.Msg}
 */
proto.Msg.deserializeBinary = function(bytes) {
  var reader = new jspb.BinaryReader(bytes);
  var msg = new proto.Msg;
  return proto.Msg.deserializeBinaryFromReader(msg, reader);
};


/**
 * Deserializes binary data (in protobuf wire format) from the
 * given reader into the given message object.
 * @param {!proto.Msg} msg The message object to deserialize into.
 * @param {!jspb.BinaryReader} reader The BinaryReader to use.
 * @return {!proto.Msg}
 */
proto.Msg.deserializeBinaryFromReader = function(msg, reader) {
  while (reader.nextField()) {
    if (reader.isEndGroup()) {
      break;
    }
    var field = reader.getFieldNumber();
    switch (field) {
    case 1:
      var value = /** @type {string} */ (reader.readString());
      msg.setDesc(value);
      break;
    case 2:
      var value = msg.getDataMap();
      reader.readMessage(value, function(message, reader) {
        jspb.Map.deserializeBinary(message, reader, jspb.BinaryReader.prototype.readString, jspb.BinaryReader.prototype.readMessage, proto.Msg.VType.deserializeBinaryFromReader, "", new proto.Msg.VType());
         });
      break;
    default:
      reader.skipField();
      break;
    }
  }
  return msg;
};


/**
 * Serializes the message to binary data (in protobuf wire format).
 * @return {!Uint8Array}
 */
proto.Msg.prototype.serializeBinary = function() {
  var writer = new jspb.BinaryWriter();
  proto.Msg.serializeBinaryToWriter(this, writer);
  return writer.getResultBuffer();
};


/**
 * Serializes the given message to binary data (in protobuf wire
 * format), writing to the given BinaryWriter.
 * @param {!proto.Msg} message
 * @param {!jspb.BinaryWriter} writer
 * @suppress {unusedLocalVariables} f is only used for nested messages
 */
proto.Msg.serializeBinaryToWriter = function(message, writer) {
  var f = undefined;
  f = message.getDesc();
  if (f.length > 0) {
    writer.writeString(
      1,
      f
    );
  }
  f = message.getDataMap(true);
  if (f && f.getLength() > 0) {
    f.serializeBinary(2, writer, jspb.BinaryWriter.prototype.writeString, jspb.BinaryWriter.prototype.writeMessage, proto.Msg.VType.serializeBinaryToWriter);
  }
};



/**
 * List of repeated fields within this message type.
 * @private {!Array<number>}
 * @const
 */
proto.Msg.VType.repeatedFields_ = [4,5];



if (jspb.Message.GENERATE_TO_OBJECT) {
/**
 * Creates an object representation of this proto.
 * Field names that are reserved in JavaScript and will be renamed to pb_name.
 * Optional fields that are not set will be set to undefined.
 * To access a reserved field use, foo.pb_<name>, eg, foo.pb_default.
 * For the list of reserved names please see:
 *     net/proto2/compiler/js/internal/generator.cc#kKeyword.
 * @param {boolean=} opt_includeInstance Deprecated. whether to include the
 *     JSPB instance for transitional soy proto support:
 *     http://goto/soy-param-migration
 * @return {!Object}
 */
proto.Msg.VType.prototype.toObject = function(opt_includeInstance) {
  return proto.Msg.VType.toObject(opt_includeInstance, this);
};


/**
 * Static version of the {@see toObject} method.
 * @param {boolean|undefined} includeInstance Deprecated. Whether to include
 *     the JSPB instance for transitional soy proto support:
 *     http://goto/soy-param-migration
 * @param {!proto.Msg.VType} msg The msg instance to transform.
 * @return {!Object}
 * @suppress {unusedLocalVariables} f is only used for nested messages
 */
proto.Msg.VType.toObject = function(includeInstance, msg) {
  var f, obj = {
    vStr: jspb.Message.getFieldWithDefault(msg, 1, ""),
    vInt: jspb.Message.getFieldWithDefault(msg, 2, 0),
    vBool: jspb.Message.getBooleanFieldWithDefault(msg, 3, false),
    vrStrList: (f = jspb.Message.getRepeatedField(msg, 4)) == null ? undefined : f,
    vrIntList: (f = jspb.Message.getRepeatedField(msg, 5)) == null ? undefined : f,
    vBytes: msg.getVBytes_asB64(),
    which: jspb.Message.getFieldWithDefault(msg, 7, 0)
  };

  if (includeInstance) {
    obj.$jspbMessageInstance = msg;
  }
  return obj;
};
}


/**
 * Deserializes binary data (in protobuf wire format).
 * @param {jspb.ByteSource} bytes The bytes to deserialize.
 * @return {!proto.Msg.VType}
 */
proto.Msg.VType.deserializeBinary = function(bytes) {
  var reader = new jspb.BinaryReader(bytes);
  var msg = new proto.Msg.VType;
  return proto.Msg.VType.deserializeBinaryFromReader(msg, reader);
};


/**
 * Deserializes binary data (in protobuf wire format) from the
 * given reader into the given message object.
 * @param {!proto.Msg.VType} msg The message object to deserialize into.
 * @param {!jspb.BinaryReader} reader The BinaryReader to use.
 * @return {!proto.Msg.VType}
 */
proto.Msg.VType.deserializeBinaryFromReader = function(msg, reader) {
  while (reader.nextField()) {
    if (reader.isEndGroup()) {
      break;
    }
    var field = reader.getFieldNumber();
    switch (field) {
    case 1:
      var value = /** @type {string} */ (reader.readString());
      msg.setVStr(value);
      break;
    case 2:
      var value = /** @type {number} */ (reader.readInt32());
      msg.setVInt(value);
      break;
    case 3:
      var value = /** @type {boolean} */ (reader.readBool());
      msg.setVBool(value);
      break;
    case 4:
      var value = /** @type {string} */ (reader.readString());
      msg.addVrStr(value);
      break;
    case 5:
      var value = /** @type {!Array<number>} */ (reader.readPackedInt32());
      msg.setVrIntList(value);
      break;
    case 6:
      var value = /** @type {!Uint8Array} */ (reader.readBytes());
      msg.setVBytes(value);
      break;
    case 7:
      var value = /** @type {number} */ (reader.readInt32());
      msg.setWhich(value);
      break;
    default:
      reader.skipField();
      break;
    }
  }
  return msg;
};


/**
 * Serializes the message to binary data (in protobuf wire format).
 * @return {!Uint8Array}
 */
proto.Msg.VType.prototype.serializeBinary = function() {
  var writer = new jspb.BinaryWriter();
  proto.Msg.VType.serializeBinaryToWriter(this, writer);
  return writer.getResultBuffer();
};


/**
 * Serializes the given message to binary data (in protobuf wire
 * format), writing to the given BinaryWriter.
 * @param {!proto.Msg.VType} message
 * @param {!jspb.BinaryWriter} writer
 * @suppress {unusedLocalVariables} f is only used for nested messages
 */
proto.Msg.VType.serializeBinaryToWriter = function(message, writer) {
  var f = undefined;
  f = message.getVStr();
  if (f.length > 0) {
    writer.writeString(
      1,
      f
    );
  }
  f = message.getVInt();
  if (f !== 0) {
    writer.writeInt32(
      2,
      f
    );
  }
  f = message.getVBool();
  if (f) {
    writer.writeBool(
      3,
      f
    );
  }
  f = message.getVrStrList();
  if (f.length > 0) {
    writer.writeRepeatedString(
      4,
      f
    );
  }
  f = message.getVrIntList();
  if (f.length > 0) {
    writer.writePackedInt32(
      5,
      f
    );
  }
  f = message.getVBytes_asU8();
  if (f.length > 0) {
    writer.writeBytes(
      6,
      f
    );
  }
  f = message.getWhich();
  if (f !== 0) {
    writer.writeInt32(
      7,
      f
    );
  }
};


/**
 * optional string v_str = 1;
 * @return {string}
 */
proto.Msg.VType.prototype.getVStr = function() {
  return /** @type {string} */ (jspb.Message.getFieldWithDefault(this, 1, ""));
};


/**
 * @param {string} value
 * @return {!proto.Msg.VType} returns this
 */
proto.Msg.VType.prototype.setVStr = function(value) {
  return jspb.Message.setProto3StringField(this, 1, value);
};


/**
 * optional int32 v_int = 2;
 * @return {number}
 */
proto.Msg.VType.prototype.getVInt = function() {
  return /** @type {number} */ (jspb.Message.getFieldWithDefault(this, 2, 0));
};


/**
 * @param {number} value
 * @return {!proto.Msg.VType} returns this
 */
proto.Msg.VType.prototype.setVInt = function(value) {
  return jspb.Message.setProto3IntField(this, 2, value);
};


/**
 * optional bool v_bool = 3;
 * @return {boolean}
 */
proto.Msg.VType.prototype.getVBool = function() {
  return /** @type {boolean} */ (jspb.Message.getBooleanFieldWithDefault(this, 3, false));
};


/**
 * @param {boolean} value
 * @return {!proto.Msg.VType} returns this
 */
proto.Msg.VType.prototype.setVBool = function(value) {
  return jspb.Message.setProto3BooleanField(this, 3, value);
};


/**
 * repeated string vr_str = 4;
 * @return {!Array<string>}
 */
proto.Msg.VType.prototype.getVrStrList = function() {
  return /** @type {!Array<string>} */ (jspb.Message.getRepeatedField(this, 4));
};


/**
 * @param {!Array<string>} value
 * @return {!proto.Msg.VType} returns this
 */
proto.Msg.VType.prototype.setVrStrList = function(value) {
  return jspb.Message.setField(this, 4, value || []);
};


/**
 * @param {string} value
 * @param {number=} opt_index
 * @return {!proto.Msg.VType} returns this
 */
proto.Msg.VType.prototype.addVrStr = function(value, opt_index) {
  return jspb.Message.addToRepeatedField(this, 4, value, opt_index);
};


/**
 * Clears the list making it empty but non-null.
 * @return {!proto.Msg.VType} returns this
 */
proto.Msg.VType.prototype.clearVrStrList = function() {
  return this.setVrStrList([]);
};


/**
 * repeated int32 vr_int = 5;
 * @return {!Array<number>}
 */
proto.Msg.VType.prototype.getVrIntList = function() {
  return /** @type {!Array<number>} */ (jspb.Message.getRepeatedField(this, 5));
};


/**
 * @param {!Array<number>} value
 * @return {!proto.Msg.VType} returns this
 */
proto.Msg.VType.prototype.setVrIntList = function(value) {
  return jspb.Message.setField(this, 5, value || []);
};


/**
 * @param {number} value
 * @param {number=} opt_index
 * @return {!proto.Msg.VType} returns this
 */
proto.Msg.VType.prototype.addVrInt = function(value, opt_index) {
  return jspb.Message.addToRepeatedField(this, 5, value, opt_index);
};


/**
 * Clears the list making it empty but non-null.
 * @return {!proto.Msg.VType} returns this
 */
proto.Msg.VType.prototype.clearVrIntList = function() {
  return this.setVrIntList([]);
};


/**
 * optional bytes v_bytes = 6;
 * @return {!(string|Uint8Array)}
 */
proto.Msg.VType.prototype.getVBytes = function() {
  return /** @type {!(string|Uint8Array)} */ (jspb.Message.getFieldWithDefault(this, 6, ""));
};


/**
 * optional bytes v_bytes = 6;
 * This is a type-conversion wrapper around `getVBytes()`
 * @return {string}
 */
proto.Msg.VType.prototype.getVBytes_asB64 = function() {
  return /** @type {string} */ (jspb.Message.bytesAsB64(
      this.getVBytes()));
};


/**
 * optional bytes v_bytes = 6;
 * Note that Uint8Array is not supported on all browsers.
 * @see http://caniuse.com/Uint8Array
 * This is a type-conversion wrapper around `getVBytes()`
 * @return {!Uint8Array}
 */
proto.Msg.VType.prototype.getVBytes_asU8 = function() {
  return /** @type {!Uint8Array} */ (jspb.Message.bytesAsU8(
      this.getVBytes()));
};


/**
 * @param {!(string|Uint8Array)} value
 * @return {!proto.Msg.VType} returns this
 */
proto.Msg.VType.prototype.setVBytes = function(value) {
  return jspb.Message.setProto3BytesField(this, 6, value);
};


/**
 * optional int32 which = 7;
 * @return {number}
 */
proto.Msg.VType.prototype.getWhich = function() {
  return /** @type {number} */ (jspb.Message.getFieldWithDefault(this, 7, 0));
};


/**
 * @param {number} value
 * @return {!proto.Msg.VType} returns this
 */
proto.Msg.VType.prototype.setWhich = function(value) {
  return jspb.Message.setProto3IntField(this, 7, value);
};


/**
 * optional string desc = 1;
 * @return {string}
 */
proto.Msg.prototype.getDesc = function() {
  return /** @type {string} */ (jspb.Message.getFieldWithDefault(this, 1, ""));
};


/**
 * @param {string} value
 * @return {!proto.Msg} returns this
 */
proto.Msg.prototype.setDesc = function(value) {
  return jspb.Message.setProto3StringField(this, 1, value);
};


/**
 * map<string, VType> data = 2;
 * @param {boolean=} opt_noLazyCreate Do not create the map if
 * empty, instead returning `undefined`
 * @return {!jspb.Map<string,!proto.Msg.VType>}
 */
proto.Msg.prototype.getDataMap = function(opt_noLazyCreate) {
  return /** @type {!jspb.Map<string,!proto.Msg.VType>} */ (
      jspb.Message.getMapField(this, 2, opt_noLazyCreate,
      proto.Msg.VType));
};


/**
 * Clears values from the map. The map will be non-null.
 * @return {!proto.Msg} returns this
 */
proto.Msg.prototype.clearDataMap = function() {
  this.getDataMap().clear();
  return this;};


goog.object.extend(exports, proto);