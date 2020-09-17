class PriceTable extends React.Component {
  static propTypes = {
    no: PropTypes.string,
    data1: PropTypes.object,
    data2: PropTypes.object,
    data3: PropTypes.object,
    isHide: PropTypes.object,
    lang: PropTypes.string,
    setStates: PropTypes.func,
    getStates: PropTypes.func
  }
  
  constructor(props) {
    super(props)
    this.handleAction = this.handleAction.bind(this)
  }
  
  handleAction() {
    
  }
  
  getText(lang) {
    var text = {
      en: {price: 'Price', bid: 'Bid', ask: 'Ask'},
      sc: {price: '现价', bid: '买入', ask: '卖出'},
      tc: {price: '現價', bid: '買入', ask: '賣出'},
    }
    return text[lang]
  }
  
  render() {
    var no = this.props.no
    var text = this.getText(this.props.lang)
    
    const priceTb = Object.entries(this.props.data2).map(([k,v])=>{
      var isCurBid = (k == this.props.data3.bid) ? 'cur_bid' : ''
      var isCurAsk = (k == this.props.data3.ask) ? 'cur_ask' : ''
      return (
        <tr key={no+'_'+k}>
          <td>{parseFloat(k).toFixed(3)}</td>
          <td className={isCurBid}>{v.bid}</td>
          <td className={isCurAsk}>{v.ask}</td>
        </tr>
      )
    })
    
    var data1 = this.props.data1, data3 = this.props.data3
    var bid = (data3.bid) ? '('+data3.bid.toFixed(3)+')' : null
    var ask = (data3.ask) ? '('+data3.ask.toFixed(3)+')': null
    var code = (data1.code.feedback.length==0 && data1.code.value.length>0) ? data1.code.value : text.price
    return(
      <div className="tab-pane fade" id={"price-table-"+no} role="tabpanel">
        <div className="price_table">
        <table className="table table-sm table-borderless table-striped table-light table-hover">
          <colgroup>
            <col span="1" width="150px" />
            <col span="1" width="150px" />
            <col span="1" width="150px" />
          </colgroup>
          <thead>
            <tr>
              <th scope="col"> {code} </th>
              <th scope="col"> {text.bid} {bid} </th>
              <th scope="col"> {text.ask} {ask} </th>
            </tr>
          </thead>
          <tbody>
            {priceTb}
          </tbody>
        </table>
        </div>
        <BtnControl
          key={"btnControlPriceTb_"+no}
          no={no}
          type={this.props.type}
          data={this.props.data1}
          lang={this.props.lang}
          isHide={this.props.isHide}
          setStates={this.props.setStates}
          getStates={this.props.getStates}
        />
      </div>
    )
  }
}