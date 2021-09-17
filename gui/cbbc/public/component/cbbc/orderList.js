class OrderList extends React.Component {
  static propTypes = {
    data: PropTypes.array,
    lang: PropTypes.string,
    setStates: PropTypes.func,
    getStates: PropTypes.func
  }
  
  constructor(props) {
    super(props)
    this.state = {}
    this.state.onOrder = []
  }
  
  static getDerivedStateFromProps(props, state) {
    var arr = []
    if (typeof props.data !== 'undefined' && props.data.length > 0) {
      for (var d of props.data)
        if (typeof d !== 'undefined' && d.length > 0)
          arr = arr.concat(d)
    }
    arr.sort(function(first, second) {
     return second.transactionTm.replace(/[^0-9]/g, '') - first.transactionTm.replace(/[^0-9]/g, '')
    })
    
    // 如果是同一时间，购买必须在前面，赎回必须在后面
    for (var i=0; i<arr.length-1; i++) {
      if (arr[i].transactionTm.replace(/[^0-9]/g, '') == arr[i+1].transactionTm.replace(/[^0-9]/g, '')) {
        if (arr[i].side=='buy' && arr[i+1].side=='sell') {
          var temp = arr[i]
          arr[i] = arr[i+1]
          arr[i+1] = temp
        }
      }
    }
    
    state.onOrder = arr
    return state
  }
  
  getText(lang) {
    var text = {
      en: {
        orderList: 'Order List', code: 'Code', filledQuantity: 'Filled Quantity', matchPrice: 'Price', matchQuantity: 'Quantity', price: 'Price', quantity: 'Quantity', rejectReason: 'Reject Reason', remainQuantity: 'Remain Quantity', result: 'Result', side: 'Side', status: 'Status', transactionTm:'Transaction Time', totalPrice: 'Turnover', onOrder: 'Order', ref: 'Ref', id: 'ID', futurePrice: 'Future Price', wtype: 'Type', issuer: 'Issuer', underlying: 'Underlying'
      },
      sc: {
        orderList: '买卖纪录', code: '牛熊证', filledQuantity: '交割单位', matchPrice: '成交价', matchQuantity: '成交单位', price: '买入价', quantity: '买入单位', rejectReason: '备注', remainQuantity: '剩馀单位', result: '结果', side: '方向', status: '状态', transactionTm:'交易时间', totalPrice: '成交额', onOrder: '成交纪录', ref: 'Ref', id: 'ID', futurePrice: '期货现价', wtype: '种类', issuer: '发行人', underlying: '标的'
      },
      tc: {
        orderList: '買賣紀錄', code: '牛熊證', filledQuantity: '交割單位', matchPrice: '成交價', matchQuantity: '成交單位', price: '買入價', quantity: '買入單位', rejectReason: '備注', remainQuantity: '剩餘單位', result: '結果', side: '方向', status: '狀態', transactionTm:'交易時間', totalPrice: '成交額', onOrder: '成交紀錄', ref: 'Ref', id: 'ID', futurePrice: '期貨現價', wtype: '種類', issuer: '發行人', underlying: '正股'
      }
    }
    return text[lang]
  }
  
  render() {
    var text = this.getText(this.props.lang)
    var rows = []
    var len = this.state.onOrder.length
    for (var i=0; i<this.state.onOrder.length; i++) {
      var d = this.state.onOrder[i]
      var style = (d.side=='buy') ? 'font-up' : (d.side=='sell') ? 'font-down' : ''
      var prefixPrice = (d.side=='') ? '' : (d.side=='sell') ? '+' : '-'
      var futurePrice = (/*d.futurePrice<100000 &&*/ d.futurePrice!=0) ? parseFloat(d.futurePrice).toFixed(2) : ''
      var reason = (d.reason.length==0 || d.reason=='') ? '' : '('+d.reason+')'
      rows.push(
        <tr key={'portfolio_'+i}>
          <td>{len-i}</td>
          <td>{d.code}</td>
          <td>{d.underlying}</td>
          <td>{d.issuer}</td>
          <td>{d.wtype}</td>
          <td>{ parseFloat(d.matchPrice).toFixed(4) }</td>
          <td>{ numberWithCommas(d.matchQuantity) }</td>
          <td>{ numberWithCommas(parseFloat(d.totalPrice).toFixed(2)) }</td>
          <td className={style}>{d.side}</td>
          <td>{ numberWithCommas(futurePrice) }</td>
          <td>{d.status} {reason}</td>
          <td>{d.transactionTm}</td>
        </tr>
      )
    }
    
    return(
      <div className='row'>
        <div className="col-12">
          <h6> {text.onOrder} </h6>
          <table className="table table-sm table-striped table-light table-order">
            <colgroup>
              <col span="1" width="50px" />
              <col span="1" width="50px" />
              <col span="1" width="100px" />
              <col span="1" width="50px" />
              <col span="1" width="50px" />
              <col span="1" width="150px" />
              <col span="1" width="150px" />
              <col span="1" width="150px" />
              <col span="1" width="150px" />
              <col span="1" width="150px" />
              <col span="1" width="300px" />
              <col span="1" width="150px" />
            </colgroup>
            <thead>
              <tr>
                <th>{text.id}</th>
                <th>{text.code}</th>
                <th>{text.underlying}</th>
                <th>{text.issuer}</th>
                <th>{text.wtype}</th>
                <th>{text.matchPrice}</th>
                <th>{text.matchQuantity}</th>
                <th>{text.totalPrice}</th>
                <th>{text.side}</th>
                <th>{text.futurePrice}</th>
                <th>{text.status}</th>
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