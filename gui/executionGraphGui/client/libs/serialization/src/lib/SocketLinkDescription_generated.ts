// automatically generated by the FlatBuffers compiler, do not modify

/**
 * @constructor
 */
export namespace executionGraph.serialization{
export class SocketLinkDescription {
  bb: flatbuffers.ByteBuffer|null = null;

  bb_pos:number = 0;
/**
 * @param number i
 * @param flatbuffers.ByteBuffer bb
 * @returns SocketLinkDescription
 */
__init(i:number, bb:flatbuffers.ByteBuffer):SocketLinkDescription {
  this.bb_pos = i;
  this.bb = bb;
  return this;
};

/**
 * @returns flatbuffers.Long
 */
outNodeId():flatbuffers.Long {
  return this.bb!.readUint64(this.bb_pos);
};

/**
 * @returns flatbuffers.Long
 */
outSocketIdx():flatbuffers.Long {
  return this.bb!.readUint64(this.bb_pos + 8);
};

/**
 * @returns flatbuffers.Long
 */
inNodeId():flatbuffers.Long {
  return this.bb!.readUint64(this.bb_pos + 16);
};

/**
 * @returns flatbuffers.Long
 */
inSocketIdx():flatbuffers.Long {
  return this.bb!.readUint64(this.bb_pos + 24);
};

/**
 * @returns boolean
 */
isWriteLink():boolean {
  return !!this.bb!.readInt8(this.bb_pos + 32);
};

/**
 * @param flatbuffers.Builder builder
 * @param flatbuffers.Long outNodeId
 * @param flatbuffers.Long outSocketIdx
 * @param flatbuffers.Long inNodeId
 * @param flatbuffers.Long inSocketIdx
 * @param boolean isWriteLink
 * @returns flatbuffers.Offset
 */
static create(builder:flatbuffers.Builder, outNodeId: flatbuffers.Long, outSocketIdx: flatbuffers.Long, inNodeId: flatbuffers.Long, inSocketIdx: flatbuffers.Long, isWriteLink: boolean):flatbuffers.Offset {
  builder.prep(8, 40);
  builder.pad(7);
  builder.writeInt8(+isWriteLink);
  builder.writeInt64(inSocketIdx);
  builder.writeInt64(inNodeId);
  builder.writeInt64(outSocketIdx);
  builder.writeInt64(outNodeId);
  return builder.offset();
};

}
}
