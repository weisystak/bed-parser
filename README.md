# bed


 provides a flexible way to define the `data lines` that are `displayed` in an `annotation track`

主要功能:  
- 可以看reads align到哪個chromosome的哪個區段  
  
其他功能(optional):
- 給 BED line名字
- 給分數
- 給方向
- 給轉譯的區段位置
- 給BED line顏色
- 給各個Exon block的位置

前三個fields是必要的, 剩下9個是optional的    
bed檔中, 每個line的field數量必須一致    
如果順位高的field有填, 就算順位低的field不想填也要填  
BED fields in custom tracks can be whitespace-delimited or tab-delimited    

只有custom tracks的開頭幾行能有header lines, 功用是來協助browser設定
這幾行的開頭都必須是browser or track    

header lines不能夠轉成`BigBed`

如果你的data很大(超過50MB), 你應該用`bigBed` data format

**chrom** - The name of the **chromosome** (e.g. chr3, chrY, chr2_random) or **scaffold** (e.g. scaffold10671).
>**scaffold**:  
In genomic mapping, a series of contigs that are in the right order but not necessarily connected in one continuous stretch of sequence.
```
[chromStart, chromEnd)  
```
**chromStart** - The first base in a chromosome is numbered 0.  
**chromEnd** - The chromEnd base is not included in the display of the feature, 是開區間

**name** - The name of the BED line.

**score** - A score between 0 and 1000.

**strand** - Defines the strand. Either "." (=no strand) or "+" or "-".

**thickStart** - The starting position at which the feature is drawn thickly (for example, the `start codon` in gene displays). 

**thickEnd** - The ending position at which the feature is drawn thickly (for example the `stop codon` in gene displays).

**itemRgb** - An RGB value of the form R,G,B (e.g. 255,0,0). If the track line itemRgb attribute is set to "On", this RBG value will determine the display color of the data contained in this BED line. 

**blockCount** - The number of blocks (exons) in the BED line.

**blockSizes** - A comma-separated list of the block sizes.

**blockStarts** - A comma-separated list of block starts. All of the blockStart positions should be calculated relative to chromStart.


e.g.  

```
track name=pairedReads description="Clone Paired Reads" useScore=1
chr22 1000 5000 cloneA 960 + 1000 5000 0 2 567,488, 0,3512
chr22 2000 6000 cloneB 900 - 2000 6000 0 2 433,399, 0,3601
```

track name=pairedReads description="Clone Paired Reads" useScore=1  

| **chrom** | **chromStart** | **chromEnd**  | **name**  | **score** | **strand** | **thickStart**  | **thickEnd** | **itemRgb**  |  **blockCount**  | **blockSizes** |  **blockStarts** |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| chr22 | 1000 | 5000 | cloneA | 960 | + | 1000 | 5000 | 0 |2 | 567,488, | 0,3512 |
| chr22 | 2000  | 6000 | cloneB | 900| - | 2000 | 6000 | 0 | 2 | 433,399, | 0,3601 |

---

# bed parser

generic bed parser 給定不同 type 來parse data  
用法請見unit_test
