class Order extends React.Component {
  constructor(props) {
    super(props)
    this.state = {}
  }
  
  componentDidMount() {

  }
  
  render() {
    var data = this.props.data,
        data2 = this.props.data2,
        isShowDataDefault = data.length==0 ? true : false,
        isShowData2Default = data2.length==0 ? true : false
    
    var noShow = 30
    if (data.length > noShow)
      data = data.slice(Math.max(data.length - noShow, 0))
    
    var rows = []
    for(var v of data) {
      if (v.side=='buy') var css='text-success'
      else if (v.side=='sell') var css='text-danger'
      else var css=''
      
      rows.push(
        <tr key={'order_'+v.orderId}>
          <td>{v.time}</td>
          <td>{v.status}</td>
          <td>{v.orderId}</td>
          <td>{v.code}</td>
          <td className={css}>{v.side}</td>
          <td>{v.quantity}</td>
          <td>{parseFloat(v.price).toFixed(3)}</td>
          <td className={css}>{parseFloat(v.matchPrice).toFixed(3)}</td>
          <td>{v.matchQuantity}</td>
          <td>{v.orderRef}</td>
        </tr>
      )
    }
    
    var rows2 = []
    for(var v of data2) {
      if (v.profit<0) var css='text-success'
      else if (v.profit>0) var css='text-danger'
      else var css=''
      
      rows2.push(
        <tr key={'profit_'+v.code}>
          <td>{v.code}</td>
          <td>{v.averagebuy}</td>
          <td>{numberWithCommas(v.buyturnover)}</td>
          <td>{v.averagesell}</td>
          <td>{numberWithCommas(v.sellturnover)}</td>
          <td></td>
          <td className={css}>{numberWithCommas(v.profit)}</td>
        </tr>
      )
    }
    
    return(
      <React.Fragment>
      
<div className="row">
<div className="col-12 col-sm-12">

<table className="table table-striped table-order">
<thead>
  <tr>
  <th colSpan="5">当日成交 (只显示最近30条订单)</th>
  <th colSpan="5"><a className="float-right" href="" onClick={(e) => {e.preventDefault()}}>下载 (显示所有订单)</a></th>
  </tr>
</thead>
<tbody>
  <tr>
  <td width="10%">时间</td>
  <td width="10%">订单<br className="d-sm-none" />状态</td>
  <td width="10%">订单<br className="d-sm-none" />号码</td>
  <td width="10%">证代码</td>
  <td width="10%">买/卖</td>
  <td width="10%">买入量</td>
  <td width="10%">盘价</td>
  <td width="10%">成交价</td>
  <td width="10%">成交量</td>
  <td width="10%">对盘<br className="d-sm-none" />牌号</td>
  </tr>
  {isShowDataDefault &&
  <tr>
  <td>-</td>
  <td>-</td>
  <td>-</td>
  <td>-</td>
  <td>-</td>
  <td>-</td>
  <td>-</td>
  <td>-</td>
  <td>-</td>
  <td>-</td>
  </tr>}
  {rows}
</tbody>
</table>

</div>
</div>


<div className="row">
<div className="col-12 col-sm-12">

<table className="table table-striped table-order">
<thead>
  <tr>
  <th colSpan="7">组合</th>
  </tr>
</thead>
<tbody>
  <tr>
  <td width="10%">证代码</td>
  <td width="10%">平均<br className="d-sm-none" />买入价</td>
  <td width="10%">买入<br className="d-sm-none" />总金额</td>
  <td width="10%">平均<br className="d-sm-none" />卖出价</td>
  <td width="10%">卖出<br className="d-sm-none" />总金额</td>
  <td width="10%">总金额</td>
  <td width="10%">盈亏</td>
  </tr>
  
  {isShowData2Default &&
  <tr>
  <td>-</td>
  <td>-</td>
  <td>-</td>
  <td>-</td>
  <td>-</td>
  <td>-</td>
  <td>-</td>
  </tr>}
  {rows2}
  
  <tr>
  <td colSpan="5">摘要</td>
  <td>0</td>
  <td>0</td>
  </tr>
</tbody>
</table>

</div>
</div>

      </React.Fragment>
    )
  }
}
