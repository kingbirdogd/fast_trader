class Position extends React.Component {
  static propTypes = {
    data: PropTypes.array,
    lang: PropTypes.string,
    setStates: PropTypes.func,
    getStates: PropTypes.func
  }
  
  constructor(props) {
    super(props)
    this.state = {}
    this.state.position = {}
  }
  
  static getDerivedStateFromProps(props, state) {
    if (typeof props.data !== 'undefined' && props.data.length > 0) {
      for (var arr of props.data) {
        if (typeof arr !== 'undefined' && arr.length > 0) {
          // code主键
          var codes = []
          for (var item of arr)
            codes.push(item.code)
          codes = Array.from(new Set(codes))
          // 重构数据集
          var datas = {}
          for (var code of codes)
            datas[code] = []
          // 注入数据
          for (var item of arr)
            datas[item.code].push(item)
          // 牛熊证，买入价，卖出价，买入单位, 卖出单位， 买入次数
          for (const [code, datas1] of Object.entries(datas)) {
            var buyTotalQuantity=0, buyTotalPrice=0, buyCount=0, buyQuantity=0, buyPrice=[]
            var sellTotalQuantity=0, sellTotalPrice=0, sellCount=0, sellQuantity=0, sellPrice=0
            var ucode=0
            for (var item1 of datas1) {
              if (item1.side=='buy') {
                buyTotalPrice += item1.matchPrice
                buyTotalQuantity += item1.matchQuantity
                buyCount += 1
                buyPrice.push(item1.matchPrice)
                buyQuantity = item1.matchQuantity
                ucode = item1.ucode
              }
              else if (item1.side=='sell') {
                sellTotalPrice += item1.matchPrice
                sellTotalQuantity += item1.matchQuantity
                sellCount += 1
                sellPrice = item1.matchPrice
                sellQuantity = item1.matchQuantity
                ucode = item1.ucode
              }
              // 手中持有倉位從其他渠道賣出
              else if (item1.side=='reset') {
                var remainQuantity = buyTotalQuantity-sellTotalQuantity
                // 如手中持有，新增一筆賣盤，以抵銷之前的倉位
                if (remainQuantity>0) {
                  sellTotalPrice += item1.matchPrice
                  sellTotalQuantity += remainQuantity
                  sellCount += 1
                  sellPrice = item1.matchPrice
                  sellQuantity = item1.remainQuantity
                  ucode = item1.ucode
                }
              }
              
              // 已平倉
              if (buyTotalQuantity-sellTotalQuantity<=0) {
                buyQuantity=0, buyPrice=[], sellQuantity=0, sellPrice=0
                delete state.position[code]
              }
              // 未平倉
              else if (buyTotalQuantity-sellTotalQuantity>0 && buyQuantity>0) {
                // 蟹左半倉
                if (buyQuantity>0 && sellQuantity>0) {}
                // 未扔貨
                else if(buyQuantity>0 && sellQuantity<=0) {}
                // 平均买入价
                var bpSum = buyPrice.reduce((a, b) => a + b, 0)
                var bpAvg = (bpSum / buyPrice.length) || 0
                
                state.position[code] = {
                  buyPrice: bpAvg,
                  buyQuantity: buyTotalQuantity-sellTotalQuantity,
                  sellPrice: sellPrice,
                  sellQuantity: sellQuantity,
                  transactionTm: item1.transactionTm,
                  buyCount: buyCount,
                  sellCount: sellCount,
                  ucode: ucode
                }
              }
            }
          }
        }
      }
    }
    return state
  }
  
  getText(lang) {
    var text = {
      en: {position: 'Position', ucode: 'Underlying', buyPrice: 'Buy Price', buyQuantity: 'Buy Quantity', sellPrice: 'Sell Price', 
            sellQuantity: 'Sell Quantity', id: 'ID', code: 'Code', transactionTm: 'Last Update Time'},
      sc: {position: '持仓', ucode: '相关资产', buyPrice: '买入价', buyQuantity: '买入单位', sellPrice: '卖出价', 
            sellQuantity: '卖出单位', id: 'ID', code: '牛熊证', transactionTm: '交易时间'},
      tc: {position: '持倉', ucode: '相關資產', buyPrice: '買入價', buyQuantity: '買入單位', sellPrice: '賣出價', 
            sellQuantity: '賣出單位', id: 'ID', code: '牛熊證', transactionTm: '交易時間'}
    }
    return text[lang]
  }
  
  render() {
    var text = this.getText(this.props.lang)
    var len = Object.keys(this.state.position).length
    var rows = []
    var no = 0
    for (const [code, d] of Object.entries(this.state.position)) {
      rows.push(
        <tr key={'position_'+no}>
          <td>{len-no}</td>
          <td>{code}</td>
          <td>{ d.ucode }</td>
          <td>{ parseFloat(d.buyPrice).toFixed(4) }</td>
          <td>{ parseFloat(d.sellPrice).toFixed(4) }</td>
          <td>{ numberWithCommas(d.buyQuantity) }</td>
          <td>{ numberWithCommas(d.sellQuantity) }</td>
          <td>{d.transactionTm}</td>
        </tr>)
      no+=1
    }
    
    return(
      <div className='row'>
        <div className="col-12">
        <h6> {text.position} </h6>
          <table className="table table-sm table-striped table-light table-position">
            <colgroup>
              <col span="1" width="50px" />
              <col span="1" width="100px" />
              <col span="1" width="100px" />
              <col span="1" width="150px" />
              <col span="1" width="150px" />
              <col span="1" width="150px" />
              <col span="1" width="150px" />
              <col span="1" width="150px" />
            </colgroup>
            <thead>
              <tr>
                <th>{text.id}</th>
                <th>{text.code}</th>
                <th>{text.ucode}</th>
                <th>{text.buyPrice}</th>
                <th>{text.sellPrice}</th>
                <th>{text.buyQuantity}</th>
                <th>{text.sellQuantity}</th>
                <th>{text.transactionTm}</th>
              </tr>
            </thead>
            <tbody>
              {rows}
            </tbody>
          </table>
        </div>
      </div>
    )
  }
}