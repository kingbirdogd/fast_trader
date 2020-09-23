class Cell extends React.Component {
  static propTypes = {
    no: PropTypes.string,
    type: PropTypes.string,
    data: PropTypes.object,
    lang: PropTypes.string,
    setStates: PropTypes.func,
    getStates: PropTypes.func
  }
  
  componentDidMount() {}
  
  getText(lang) {
    var text = {
      en: { bull: 'Bull', bear: 'Bear', priceTable: 'Price Table', setting: 'Setting', trade: 'Trade'},
      sc: { bull: '牛证', bear: '熊证', priceTable: '报价表', setting: '设置', trade: '买入/卖出'},
      tc: { bull: '牛證', bear: '熊證', priceTable: '報價表', setting: '設置', trade: '買入/賣出'}
    }
    return text[lang]
  }
  
  render() {
    var no = this.props.no
    var text = this.getText(this.props.lang)
    var isHide1 = {set: false, start: false, pause: false, stop: false}
    var isHide2 = {set: true, start: false, pause: false, stop: true}
    
    return(
      <div className="col-12 col-sm-6 col-md-3 mb-1">
        <ul className="nav nav-tabs" id="cbbc_tab_001" role="tablist">
          <li className="nav-item"> <a className="nav-link active" data-toggle="tab" href={"#type-"+no} role="tab" aria-selected="true"> {text[this.props.type]} </a> </li>
          <li className="nav-item"> <a className="nav-link" data-toggle="tab" href={"#price-table-"+no} role="tab" aria-selected="false"> {text.priceTable} </a> </li>
          <li className="nav-item"> <a className="nav-link" data-toggle="tab" href={"#setting-"+no} role="tab" aria-selected="false"> {text.setting} </a> </li>
          <li className="nav-item"> <a className="nav-link" data-toggle="tab" href={"#trade-"+no} role="tab" aria-selected="false"> {text.trade} </a> </li>
        </ul>
        
        <div className="tab-content" id="cbbc_tab_content">
          <Action key={"action_"+no}
            no={no}
            type={this.props.type}
            data={this.props.data.action}
            isHide={isHide1}
            lang={this.props.lang}
            setStates={this.props.setStates}
            getStates={this.props.getStates}
          />
          <PriceTable
            key={"priceTable_"+no}
            no={no}
            type={this.props.type}
            data1={this.props.data.action}
            data2={this.props.data.priceTable}
            data3={this.props.data.wPrice}
            isHide={isHide2}
            lang={this.props.lang}
            setStates={this.props.setStates}
            getStates={this.props.getStates}
          />
          <Setting
            key={"setting_"+no}
            type={this.props.type}
            no={no}
            data1={this.props.data.action}
            data2={this.props.data.setting}
            isHide={isHide2}
            lang={this.props.lang}
            setStates={this.props.setStates}
            getStates={this.props.getStates}
          />
          <Trade
            key={"trade_"+no}
            type={this.props.type}
            no={no}
            data1={this.props.data.action}
            data2={this.props.data.trade}
            isHide={isHide2}
            lang={this.props.lang}
            setStates={this.props.setStates}
            getStates={this.props.getStates}
          />
        </div>
        
      </div>
    )
  }
}