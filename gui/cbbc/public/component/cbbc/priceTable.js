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
    this.handleClick = this.handleClick.bind(this)
  }
  
  handleClick() {
    var no = event.target.attributes.getNamedItem('data-no').value
    var name = event.target.name
    
    var states = this.props.getStates()
    var obj = $.extend(true, [], states.cells),
        obj2 = $.extend(true, [], states.cells[no].action)
    
    obj[no].priceTable = {}
    this.props.setStates({cells: obj})
    
    var command = {
      cmd: "loadpricetable",
      action: "loadpricetable",
      warrant_code: parseInt(obj2.code.value),
      ref: states.prefix+no,
      id: parseInt(states.userId),
      algo_name: states.modules[states.cells[no].type]
    }
    sendWebsocket(JSON.stringify(command))
  }
  
  componentDidUpdate() {
    ui.tablePrice = $(".price_table");
    var obj = ui.tablePrice.niceScroll({
      cursorcolor: '#797979',
      autohidemode: true,
      cursorwidth: 8
    });
    
    ui.tablePrice.mouseover(function(){
      ui.tablePrice.getNiceScroll().resize();
    });
  }
  
  getText(lang) {
    var text = {
      en: {price: 'Price', bid: 'Bid', ask: 'Ask', loadPrice: 'Load Price Table'},
      sc: {price: '现价', bid: '买入', ask: '卖出', loadPrice: '抓取报价盘口'},
      tc: {price: '現價', bid: '買入', ask: '賣出', loadPrice: '抓取報價盤口'},
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
        
        <div className="btn_trade_control">
          <div className="form-group row">
            <div className="col-6 col-sm-6 mt-0 mt-sm-0">
              <button name="loadPrice" type="button" class="btn btn-sm btn-block btn-secondary" data-no={no} onClick={this.handleClick}>{text.loadPrice}</button>
            </div>
          </div>
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